// Copyright (c) 2022, Marcio Lucca
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "mdl/concurrent.h"

#include <gtest/gtest.h>
#include <string>
#include <thread>

using std::cout;
using std::endl;

namespace mdl {
namespace concurrent {
namespace futuretestsuite {
template<class T>
  class TestFuture : public Future<T> {
    public:
      bool Start() override {
        return Future<T>::Start();
      }
      void Set(T&& value) override {
        Future<T>::Set(std::forward<T>(value));
      }
      void SetError(const std::string& errorMsg, int errorCode) override {
        Future<T>::SetError(errorMsg, errorCode);
      }
  };


  TEST(FutureTestSuite, FutureTest_SingleThread_Set) {
    TestFuture<int> future;
    ASSERT_FALSE(future.IsCanceled());
    ASSERT_FALSE(future.IsDone());

    future.Set(10);
  
    ASSERT_FALSE(future.IsCanceled());
    ASSERT_TRUE(future.IsDone());
    ASSERT_EQ(10, future.Get());

    ASSERT_FALSE(future.Cancel());

    ASSERT_FALSE(future.IsCanceled());
    ASSERT_TRUE(future.IsDone());
    ASSERT_EQ(10, future.Get());

    future.Set(20);
    ASSERT_FALSE(future.IsCanceled());
    ASSERT_TRUE(future.IsDone());
    ASSERT_EQ(10, future.Get());

    future.SetError("Not good", 1);
    ASSERT_FALSE(future.IsCanceled());
    ASSERT_TRUE(future.IsDone());
    ASSERT_EQ(10, future.Get());
  }

  TEST(FutureTestSuite, FutureTest_SingleThread_Error) {
    TestFuture<int> future;
    ASSERT_FALSE(future.IsCanceled());
    ASSERT_FALSE(future.IsDone());

    future.SetError("Not good", 2);
  
    ASSERT_FALSE(future.IsCanceled());
    ASSERT_TRUE(future.IsDone());
    
    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_NE("Not good", ex.what());
      ASSERT_STREQ("Not good", ex.what());
      ASSERT_EQ(2, ex.what_code());
    }

    ASSERT_FALSE(future.Cancel());
    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_NE("Not good", ex.what());
      ASSERT_STREQ("Not good", ex.what());
      ASSERT_EQ(2, ex.what_code());
    }

    future.Set(20);
    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_NE("Not good", ex.what());
      ASSERT_STREQ("Not good", ex.what());
      ASSERT_EQ(2, ex.what_code());
    }

    future.SetError("A different error", 0);
    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_NE("Not good", ex.what());
      ASSERT_STREQ("Not good", ex.what());
      ASSERT_EQ(2, ex.what_code());
    }
  }

  TEST(FutureTestSuite, FutureTest_SingleThread_GetOrSetAfterCancelled) {
    TestFuture<int> future;
    ASSERT_FALSE(future.IsCanceled());
    ASSERT_FALSE(future.IsDone());

    ASSERT_TRUE(future.Cancel());
  
    ASSERT_TRUE(future.IsCanceled());
    ASSERT_TRUE(future.IsDone());
    
    try {
      future.Get();
      FAIL();
    } catch (const interrupted_exception& ex) {
      ASSERT_STREQ("Task has been cancelled", ex.what());
    }

    future.Set(20);

    try {
      future.Get();
      FAIL();
    } catch (const interrupted_exception& ex) {
      ASSERT_STREQ("Task has been cancelled", ex.what());
    }

    future.SetError("Not good", 3);

    try {
      future.Get();
      FAIL();
    } catch (const interrupted_exception& ex) {
      ASSERT_STREQ("Task has been cancelled", ex.what());
    }

    ASSERT_TRUE(future.Cancel());
  }

  template <class T>
  void WaitAndSet(T&& value, TestFuture<T> future) {
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    future.Set(std::forward<T>(value));
  }

  TEST(FutureTestSuite, FutureTest_MultiThread_Get) {
    TestFuture<int> future;
    std::thread t1(WaitAndSet<int>, 10, future);

    ASSERT_EQ(10, future.Get());
    t1.join();
  }

  template <class T>
  void WaitAndCancel(TestFuture<T> future) {
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    future.Cancel();
  }

  TEST(FutureTestSuite, FutureTest_MultiThread_Cancel) {
    TestFuture<int> future;
    std::thread t1(WaitAndCancel<int>, future);

    ASSERT_THROW(future.Get(), interrupted_exception);
    t1.join();
  }

  template <class T>
  void WaitAndSetError(TestFuture<T> future, int errorCode, std::string errorMsg) {
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    future.SetError(errorMsg, errorCode);
  }

  TEST(FutureTestSuite, FutureTest_MultiThread_SetError) {
    TestFuture<int> future;
    std::thread t1(WaitAndSetError<int>, future, 20, "No good");

    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_EQ(20, ex.what_code());
      ASSERT_STREQ("No good", ex.what());
    }
    
    t1.join();
  }
} // futuretestsuite
} // concurrent
} // mdl

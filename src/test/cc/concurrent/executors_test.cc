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

#include <mdl/concurrent.h>

#include <gtest/gtest.h>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::cout;
using std::endl;

namespace mdl {
namespace concurrent {
namespace executorstest {
  TEST(ExecutorsTestSuite, ExecutorsTest_TestExecute) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(5, factory);
    std::mutex mutex;
    std::unordered_map<std::string, int> counts;

    for (int i = 0; i < 10; i++) {
      executor.Execute([&mutex, &counts]() {
        this_thread::sleep(10);
        std::lock_guard<std::mutex> guard(mutex);
        counts[this_thread::get_name()]++;
      });
    }

    // allow jobs to finish
    this_thread::sleep(100);
    executor.Shutdown();

    ASSERT_EQ(2, counts["my-thread-1"]);
    ASSERT_EQ(2, counts["my-thread-2"]);
    ASSERT_EQ(2, counts["my-thread-3"]);
    ASSERT_EQ(2, counts["my-thread-4"]);
    ASSERT_EQ(2, counts["my-thread-5"]);
  }

  TEST(ExecutorsTestSuite, ExecutorsTest_TestSubmit) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(5, factory);
    std::vector<Future<int>> futures;

    for (int i = 0; i < 10; i++) {
      futures.push_back(executor.Submit<int>([i]() {
        this_thread::sleep(10);
        return i;
      }));
      futures.push_back(executor.Submit<int>([i]() {
        this_thread::sleep(10);
        return 1000 + i;
      }));
    }

    std::unordered_set<int> values;
    for (auto it = futures.begin(); it != futures.end(); it++) {
      values.insert(it->Get());
    }

    ASSERT_EQ(20, values.size());
    for (int i = 0; i < 10; i++) {
      ASSERT_TRUE(values.count(i));
      ASSERT_TRUE(values.count(1000 + i));
    }

    executor.Shutdown();
  }

  struct X {
    int val;
    X() {}
    X(int val) : val(val) {}
    X(const X& other) : val(other.val) {
      cout << "Copy constructor: " << val << endl;
    }
    X(X&& other) : val(other.val) {
      cout << "Move constructor: " << val << endl;
    }
    X& operator=(const X& other) { 
      val = other.val; 
      cout << "Copy assignment: " << val << endl;
      return *this; 
    }
    X& operator=(X&& other) { 
      val = other.val; 
      cout << "Move assignment: " << val << endl;
      return *this; 
    }
  };

  TEST(ExecutorsTestSuite, ExecutorsTest_TestSubmitObj) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(1, factory);
    Future<X> future = executor.Submit<X>([]() {
      return X(10);
    });

    ASSERT_EQ(10, future.Get().val);
  }

  TEST(ExecutorsTestSuite, ExecutorsTest_TestSubmitObj2) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(1, factory);
    Future<X> future = executor.Submit<X>([]() {
      X local(11);
      return local;
    });

    ASSERT_EQ(11, future.Get().val);
  }

  TEST(ExecutorsTestSuite, ExecutorsTest_TestSubmit_Throws) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(1, factory);
    Future<int> future = executor.Submit<int>([]() {
      if (true) { throw 100; }
      return 10;
    });

    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_EQ(100, ex.what_code());
      ASSERT_STREQ("Failed to execute task", ex.what());
    }
  }

  TEST(ExecutorsTestSuite, ExecutorsTest_TestSubmit_Throws_Exception) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(1, factory);
    Future<int> future = executor.Submit<int>([]() {
      if (true) { throw std::runtime_error("Bad method"); }
      return 10;
    });

    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_EQ(-1, ex.what_code());
      ASSERT_STREQ("Bad method", ex.what());
    }
  }

  TEST(ExecutorsTestSuite, ExecutorsTest_TestSubmit_Throws_String) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(1, factory);
    Future<int> future = executor.Submit<int>([]() {
      if (true) { throw std::string("Bad method"); }
      return 10;
    });

    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_EQ(-1, ex.what_code());
      ASSERT_STREQ("Bad method", ex.what());
    }
  }

  TEST(ExecutorsTestSuite, ExecutorsTest_TestSubmit_Throws_ConstCharPtr) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(1, factory);
    Future<int> future = executor.Submit<int>([]() {
      if (true) { throw "Bad method"; }
      return 10;
    });

    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_EQ(-1, ex.what_code());
      ASSERT_STREQ("Bad method", ex.what());
    }
  }

  TEST(ExecutorsTestSuite, ExecutorsTest_TestSubmit_Throws_ExecutionException) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(1, factory);
    Future<int> future = executor.Submit<int>([]() {
      if (true) { throw execution_exception("Bad method", 7); }
      return 10;
    });

    try {
      future.Get();
      FAIL();
    } catch (const execution_exception& ex) {
      ASSERT_EQ(7, ex.what_code());
      ASSERT_STREQ("Bad method", ex.what());
    }
  }

  TEST(ExecutorsTestSuite, ExecutorsTest_TestSubmit_Cancel) {
    ThreadFactory factory("my-thread");
    ExecutorService executor(1, factory);
    Synchronizable sync;

    executor.Execute([&sync]() {
      // blocks the one thread
      sync.Synchronized<void>([&sync]() {
        sync.Wait();
      });
    });

    bool executed = false;
    // this will remain enqueued
    Future<int> future = executor.Submit<int>([&executed]() {
      executed = true;
      return 100;
    });

    this_thread::sleep(100);

    // cancel the enqueued task
    ASSERT_TRUE(future.Cancel());

    sync.Synchronized<void>([&sync]() {
      sync.Notify();
    });

    this_thread::sleep(100);
    executor.Shutdown();

    ASSERT_TRUE(future.IsCanceled());
    ASSERT_FALSE(executed);
  }

} // threadtest
} // concurrent
} // mdl

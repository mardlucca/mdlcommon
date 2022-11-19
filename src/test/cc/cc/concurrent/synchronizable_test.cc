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

#include <exception>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "mdl/concurrent.h"

using std::cout;
using std::endl;

namespace mdl {
namespace concurrent {
namespace syncronizabletest {
  
  class X : public Synchronizable {
    public:
      int x = 0;
      void Set(int val) {
        Synchronized<void>([this, val]() {
          std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(500));
          if (val < 0) { throw std::runtime_error("Negative error"); }
          x = val;
          if (x % 2) {
            NotifyAll();
          } else {
            Notify();
          }
        });
      }

      int Get() {
        return Synchronized<int>([this]() {
          return x;
        });
      }
  };

  TEST(SynchronizableTestSuite, TestSynchronized_SingleThread) {
    Synchronizable sync;
    int result = sync.Synchronized<int>([]() {return 10;});
    ASSERT_EQ(10, result);
  }

  TEST(SynchronizableTestSuite, TestSynchronized_SingleThread_Reentrant) {
    Synchronizable sync;
    int result = sync.Synchronized<int>([&sync]() {
      return sync.Synchronized<int>([]() { return 20; });
    });
    ASSERT_EQ(20, result);
  }

  TEST(SynchronizableTestSuite, TestSynchronized_MultiThread) {
    X sync;
    std::thread t1(&X::Set, &sync, 20);
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    ASSERT_EQ(20, sync.Get());
    t1.join();
    sync.x = 30;
    ASSERT_EQ(30, sync.Get());
  }

  void SetNegative(X& x, int val) {
    try {
      x.Set(val);
    } catch (std::runtime_error& error) {}
  }

  TEST(SynchronizableTestSuite, TestSynchronized_MultiThread_Throwing) {
    X sync;
    std::thread t1(SetNegative, std::ref(sync), -20);
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    ASSERT_EQ(0, sync.Get());
    t1.join();
    sync.x = 30;
    ASSERT_EQ(30, sync.Get());
  }

  void Wait(X& sync, int val, bool& done) {
    sync.Synchronized<void>([&sync, val, &done]() {
      while (sync.x != val) {
        sync.Wait();
      }
      done = true;
    });
  }

  TEST(SynchronizableTestSuite, TestWaitNotify_MultiThread) {
    X sync;
    bool d1 = false;
    bool d2 = false;
    bool d3 = false;
    int val = 20;
    std::thread t1(Wait, std::ref(sync), val, std::ref(d1));
    std::thread t2(Wait, std::ref(sync), val, std::ref(d2));
    std::thread t3(Wait, std::ref(sync), val, std::ref(d3));
    
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    sync.Set(val);

    ASSERT_EQ(val, sync.Get());
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    ASSERT_EQ(1, d1 + d2 + d3);

    sync.Set(val);
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    ASSERT_EQ(2, d1 + d2 + d3);

    sync.Set(val);
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    ASSERT_EQ(3, d1 + d2 + d3);

    t1.join();
    t2.join();
    t3.join();
  }

  TEST(SynchronizableTestSuite, TestWaitNotifyAll_MultiThread) {
    X sync;
    bool d1 = false;
    bool d2 = false;
    bool d3 = false;
    int val = 21;
    std::thread t1(Wait, std::ref(sync), val, std::ref(d1));
    std::thread t2(Wait, std::ref(sync), val, std::ref(d2));
    std::thread t3(Wait, std::ref(sync), val, std::ref(d3));
    
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    sync.Set(val);
    ASSERT_EQ(val, sync.Get());

    t1.join();
    t2.join();
    t3.join();

    ASSERT_TRUE(d1);
    ASSERT_TRUE(d2);
    ASSERT_TRUE(d3);
  }

  void InterruptibleWait(X& sync, bool& done) {
    sync.Synchronized<void>([&sync, &done]() {
      try {
        sync.Wait();
        FAIL();
      } catch (interrupted_exception& ex) {
        done = true;
      }
    });    
  }

  TEST(SynchronizableTestSuite, TestInterrupt_MultiThread) {
    X sync;
    bool d1 = false;
    bool d2 = false;
    std::thread t1(InterruptibleWait, std::ref(sync), std::ref(d1));
    std::thread t2(InterruptibleWait, std::ref(sync), std::ref(d2));
    
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(200));
    ASSERT_FALSE(d1);
    ASSERT_FALSE(d2);

    sync.Synchronized<void>([&sync]() {
      sync.Interrupt();
    });

    t1.join();
    t2.join();
    ASSERT_TRUE(d1);
    ASSERT_TRUE(d2);

    // test that object sync still usable
    bool d3 = false;
    std::thread t3(InterruptibleWait, std::ref(sync), std::ref(d3));
    
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(200));
    ASSERT_FALSE(d3);

    sync.Synchronized<void>([&sync]() {
      sync.Interrupt();
    });

    t3.join();
    ASSERT_TRUE(d3);
  }
} // synchronizabletest
} // concurrent
} // mdl

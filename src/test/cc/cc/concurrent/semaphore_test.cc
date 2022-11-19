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
#include <unordered_set>
#include <mutex>

#include "mdl/concurrent.h"
#include "mdl/util.h"

using std::cout;
using std::endl;

namespace mdl {
namespace concurrent {
namespace semaphoretest {
  std::mutex mutex;
  void Consume(Semaphore& s, int n, std::unordered_set<int>& set) {
    while (n > 0) {
      s.Down();
      auto guard = std::lock_guard<std::mutex>(mutex);
      set.insert(n);
      n--;
    }
  }

  TEST(SemaphoreTestSuite, TestSemaphore_SingleThread) {
    Semaphore s(4);
    std::unordered_set<int> set;
    s.Up();
    Consume(s, 5, set);
    ASSERT_EQ(5, set.size());
  }

  TEST(SemaphoreTestSuite, TestSemaphore_MultiThread) {
    Semaphore s(2);
    s.InterruptAll();     //no-op
    std::unordered_set<int> set;

    std::thread t1(Consume, std::ref(s), 5, std::ref(set));

    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    ASSERT_EQ(2, set.size());

    s.Up();
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    ASSERT_EQ(3, set.size());

    s.Up();
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    ASSERT_EQ(4, set.size());
    ASSERT_EQ(0, set.count(1));

    s.Up();
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    ASSERT_EQ(5, set.size());
    ASSERT_EQ(1, set.count(1));

    t1.join();
  }  


  void DelayedConsume(Semaphore& s, int n, std::unordered_set<int>& set, long delay) {
    while (n > 0) {
      s.Down<void>([delay](int numTickets) {
        std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(delay));
      });

      auto guard = std::lock_guard<std::mutex>(mutex);
      set.insert(n);
      n--;
    }
  }
  
  TEST(SemaphoreTestSuite, TestSemaphore_MultiThread_WithCallback) {
    Semaphore s(0);
    std::unordered_set<int> set;
    int num = 10;
    int delay = 100;

    std::thread t1(DelayedConsume, std::ref(s), num, std::ref(set), delay);
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(10));

    auto start = mdl::util::Now();
    for (int i = 0; i < num; i++) {
      // because consuming is delayed in the sync block, producing must wait for the mutex.
      // producing needs to give consumer a chance to run
      std::this_thread::sleep_for(std::chrono::duration<long, std::micro>(10));
      // std::this_thread::yield();
      s.Up<void>([](int numTickets) {});
    }
    auto end = mdl::util::Now();

    t1.join();
    long time = mdl::util::EllapsedTime(start, end) / 1000L;
    cout << "Ellapsed Time: " << time << endl;
    ASSERT_TRUE(time > (num - 1) * delay && time < (num + num/20) * delay);
  }  

  void InterruptibleConsume(Semaphore& s, int n, std::unordered_set<int>& set) {
    try {
      s.Down();
      FAIL();
    } catch (interrupted_exception& ex) {
      auto guard = std::lock_guard<std::mutex>(mutex);
      set.insert(n);
    }
  }

  TEST(SemaphoreTestSuite, TestSemaphore_InterruptAll) {
    Semaphore s(0);
    std::unordered_set<int> set;

    std::thread t1(InterruptibleConsume, std::ref(s), 10, std::ref(set));
    std::thread t2(InterruptibleConsume, std::ref(s), 20, std::ref(set));

    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(100));
    
    ASSERT_EQ(0, set.size());
    ASSERT_EQ(-2, s.NumTickets());

    s.InterruptAll();

    t1.join();
    t2.join();

    ASSERT_EQ(2, set.size());
    ASSERT_TRUE(set.count(10));
    ASSERT_TRUE(set.count(20));
    ASSERT_EQ(0, s.NumTickets());
  }

} // semaphoretest
} // concurrent
} // mdl

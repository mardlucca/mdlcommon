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

#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_set>

#include "mdl/concurrent.h"

using std::cout;
using std::endl;

namespace mdl {
namespace concurrent {
namespace threadlocaltest {
  static std::unordered_set<int> destructors;
  static std::mutex m1;

  class Object {
    public:
      int val;

      Object() : Object(0) {}
      Object(int val) : val(val) {}
      ~Object() { 
        auto g = std::lock_guard<std::mutex>(m1);
        destructors.insert(val); 
      }
  };

  TEST(ThreadLocalTestSuite, TestGetSet) {
    ThreadLocal<int> tl;
    ASSERT_FALSE(tl);
    ASSERT_EQ(nullptr, tl.Get());

    {
        auto guard = tl.Set(new int(20));
        ASSERT_TRUE(tl);
        ASSERT_EQ(20, *tl);
        ASSERT_EQ(20, *tl.Get());

        *tl = 30;
        ASSERT_EQ(30, *tl);

        {
          auto guard2 = tl.Set(new int(40));
        }

        ASSERT_TRUE(tl);
        ASSERT_EQ(40, *tl);
    }

    ASSERT_FALSE(tl);
    ASSERT_EQ(nullptr, tl.Get());
  }

  
  TEST(ThreadLocalTestSuite, TestObject) {
    destructors.clear();

    ThreadLocal<Object> tl;
    ASSERT_FALSE(tl);
    ASSERT_EQ(nullptr, tl.Get());

    ASSERT_EQ(0, destructors.size());

    {
      auto guard = tl.Set(new Object(10));
      ASSERT_TRUE(tl);
      ASSERT_EQ(10, tl.Get()->val);
      ASSERT_EQ(10, tl->val);
      ASSERT_EQ(0, destructors.count(1));

      {
        auto guard2 = tl.Set(new Object(20));
        ASSERT_TRUE(tl);
        ASSERT_EQ(20, tl->val);
        ASSERT_EQ(1, destructors.count(10));
      }

      {
        auto guard2 = tl.Set(nullptr);
        ASSERT_FALSE(tl);
        ASSERT_EQ(1, destructors.count(20));
      }

      {
        // ... since this is a nested guard, it will not unbind the thread value
        auto guard2 = tl.Set(new Object(30));
      }

      // ... so value is still 30 here.
      ASSERT_TRUE(tl);
      ASSERT_EQ(30, tl->val);
      ASSERT_EQ(0, destructors.count(30));
    }

    ASSERT_EQ(1, destructors.count(10));
    ASSERT_EQ(1, destructors.count(20));
    ASSERT_EQ(1, destructors.count(30));
    ASSERT_FALSE(tl);
    ASSERT_EQ(nullptr, tl.Get());
  }

  TEST(ThreadLocalTestSuite, TestArray) {
    destructors.clear();
    ThreadLocal<Object[]> tl;

    {
      Object * xs = new Object[3];
      xs[0].val = 10;
      xs[1].val = 20;
      xs[2].val = 30;
      ASSERT_FALSE(tl);
      auto guard = tl.Set(xs);
      ASSERT_TRUE(tl);

      ASSERT_EQ(10, tl.Get()[0].val);
      ASSERT_EQ(20, tl.Get()[1].val);
      ASSERT_EQ(30, tl.Get()[2].val);

      ASSERT_EQ(10, tl[0].val);
      ASSERT_EQ(20, tl[1].val);
      ASSERT_EQ(30, tl[2].val);
    }

    ASSERT_FALSE(tl);
    ASSERT_EQ(3, destructors.size());
    ASSERT_EQ(1, destructors.count(10));
    ASSERT_EQ(1, destructors.count(20));
    ASSERT_EQ(1, destructors.count(30));
  }

  void func(ThreadLocal<Object>& tl, int id, int numThreads, int count) {
    auto guard = tl.Set(new Object(id));
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(50));
    for (int i = 0; i < count; i++) {
      tl->val += (numThreads + 1);
      ASSERT_EQ(id, tl->val % (numThreads + 1));
      std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(50));
    }
  }

  TEST(ThreadLocalTestSuite, TestMultiThread) {
    destructors.clear();
    ThreadLocal<Object> tl;
    int count = 20;
    int numThreads = 5;
    
    std::thread thread1(func, std::ref(tl), 1, numThreads, count);
    std::thread thread2(func, std::ref(tl), 2, numThreads, count);
    std::thread thread3(func, std::ref(tl), 3, numThreads, count);
    std::thread thread4(func, std::ref(tl), 4, numThreads, count);
    std::thread thread5(func, std::ref(tl), 5, numThreads, count);
    
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();

    ASSERT_EQ(5, destructors.size());
    ASSERT_EQ(1, destructors.count(1 + (numThreads + 1) * count));
    ASSERT_EQ(1, destructors.count(2 + (numThreads + 1) * count));
    ASSERT_EQ(1, destructors.count(3 + (numThreads + 1) * count));
    ASSERT_EQ(1, destructors.count(4 + (numThreads + 1) * count));
    ASSERT_EQ(1, destructors.count(5 + (numThreads + 1) * count));

  }

} // threadlocaltest
} // concurrent
} // mdl

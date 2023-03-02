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
#include <iostream>
#include <thread>
#include <unordered_map>

using std::cout;
using std::endl;

namespace mdl {
namespace concurrent {
namespace threadtest {
  void Sleep(int& time) {
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(time));
    ASSERT_EQ("Thread 1", this_thread::get_name());
    time = -time;
  }

  TEST(ThreadTestSuite, TestThread_NamedThread) {
    int time = 100;
    std::thread t1 = named_thread("Thread 1", Sleep, std::ref(time));
    ASSERT_TRUE(t1.joinable());
    ASSERT_EQ("", this_thread::get_name());
    ASSERT_NE(std::thread::id(), t1.get_id());
    t1.join();

    ASSERT_EQ(-std::abs(time), time);
  }

  void AssertName(const std::string& name, bool& done) {
    ASSERT_EQ(name, this_thread::get_name());
    done = true;
  }

  TEST(ThreadTestSuite, TestThread_ThreadFactory) {
    ThreadFactory factory("threads");
    bool d1 = false;
    bool d2 = false;
    bool d3 = false;

    std::thread t1 = factory.NewThread(AssertName, std::string("threads-1"), std::ref(d1));
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(20));

    std::thread t2 = factory.NewThread(AssertName, std::string("threads-2"), std::ref(d2));
    std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(20));

    std::thread t3 = factory.NewThread(AssertName, std::string("threads-3"), std::ref(d3));

    t1.join();
    t2.join();
    t3.join();

    ASSERT_TRUE(d1);
    ASSERT_TRUE(d2);
    ASSERT_TRUE(d3);
  }

} // threadtest
} // concurrent
} // mdl

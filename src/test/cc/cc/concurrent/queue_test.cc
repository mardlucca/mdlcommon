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

#include "mdl/concurrent.h"

#include <mutex>

using std::cout;
using std::endl;

namespace mdl {
namespace concurrent {
namespace queuetest {

  struct Counters {
      int cntConstructor = 0;
      int cntCopyConstructor = 0;
      int cntMoveConstructor = 0;
      int cntDestructor = 0;
  }; 

  class X {
    public:
      int id;

      X(int id) {
        this->id = id;
      }
      X(const X& x) {
        id = x.id;
      }
      X(X&& x) {
        id = x.id;
      }
      ~X() {
      }
  };

  TEST(QueueTestSuite, TestQueue_Primitive) {
    Queue<int> queue;
    ASSERT_EQ(0, queue.Size());

    queue.Add(10);
    queue.Add(20);
    queue.Add(30);
    ASSERT_EQ(3, queue.Size());

    ASSERT_EQ(10, queue.Poll());
    ASSERT_EQ(2, queue.Size());

    ASSERT_EQ(20, queue.Poll());
    ASSERT_EQ(1, queue.Size());

    ASSERT_EQ(30, queue.Poll());
    ASSERT_EQ(0, queue.Size());
  }

  TEST(QueueTestSuite, TestQueue_ObjByVal) {
    Queue<X> queue;
    ASSERT_EQ(0, queue.Size());

    queue.Add(X(10));
    queue.Add(X(20));
    queue.Add(X(30));

    ASSERT_EQ(3, queue.Size());

    Queue<X> queue2 = queue;
    ASSERT_EQ(3, queue2.Size());

    ASSERT_EQ(10, queue.Poll().id);
    ASSERT_EQ(2, queue.Size());

    ASSERT_EQ(20, queue.Poll().id);
    ASSERT_EQ(1, queue.Size());

    ASSERT_EQ(30, queue.Poll().id);
    ASSERT_EQ(0, queue.Size());

    ASSERT_EQ(3, queue2.Size());
  }

  TEST(QueueTestSuite, TestQueue_ObjPtr) {
    Queue<X*> queue;
    ASSERT_EQ(0, queue.Size());

    queue.Add(new X(10));
    queue.Add(new X(20));
    queue.Add(new X(30));
    ASSERT_EQ(3, queue.Size());

    X* val = queue.Poll();
    ASSERT_EQ(10, val->id);
    ASSERT_EQ(2, queue.Size());
    delete val;

    val = queue.Poll();
    ASSERT_EQ(20, val->id);
    ASSERT_EQ(1, queue.Size());
    delete val;

    val = queue.Poll();
    ASSERT_EQ(30, val->id);
    ASSERT_EQ(0, queue.Size());
    delete val;
  }


  TEST(QueueTestSuite, TestQueue_UniquePtr) {
    cout << std::is_copy_constructible<std::unique_ptr<X>>::value << endl;
    cout << std::is_copy_constructible<X>::value << endl;
    Queue<std::unique_ptr<X>> queue;
    ASSERT_EQ(0, queue.Size());

    queue.Add(std::unique_ptr<X>(new X(10)));
    queue.Add(std::unique_ptr<X>(new X(20)));
    queue.Add(std::unique_ptr<X>(new X(30)));
    ASSERT_EQ(3, queue.Size());

    ASSERT_EQ(10, queue.Poll()->id);
    ASSERT_EQ(2, queue.Size());

    ASSERT_EQ(20, queue.Poll()->id);
    ASSERT_EQ(1, queue.Size());

    ASSERT_EQ(30, queue.Poll()->id);
    ASSERT_EQ(0, queue.Size());
  }

  TEST(QueueTestSuite, TestQueue_PollEmpty) {
    Queue<int> queue;
    ASSERT_THROW(queue.Poll(), mdl::util::not_found_exception);
    ASSERT_THROW(queue.Poll(), mdl::util::not_found_exception);
    ASSERT_THROW(queue.Poll(), mdl::util::not_found_exception);

    queue.Add(10);
    ASSERT_EQ(10, queue.Poll());

    ASSERT_THROW(queue.Poll(), mdl::util::not_found_exception);
    ASSERT_THROW(queue.Poll(), mdl::util::not_found_exception);
  }

  TEST(QueueTestSuite, TestSynchronizedQueue_Primitive) {
    SynchronizedQueue<int> queue;

    ASSERT_EQ(0, queue.Size());

    queue.Add(10);
    queue.Add(20);
    queue.Add(30);
    ASSERT_EQ(3, queue.Size());

    SynchronizedQueue<int> queue2 = queue;
    ASSERT_EQ(3, queue2.Size());

    SynchronizedQueue<int> queue3 = queue2;
    ASSERT_EQ(3, queue3.Size());

    ASSERT_EQ(10, queue.Poll());
    ASSERT_EQ(2, queue.Size());

    ASSERT_EQ(20, queue.Poll());
    ASSERT_EQ(1, queue.Size());

    ASSERT_EQ(30, queue.Poll());
    ASSERT_EQ(0, queue.Size());
  }

  TEST(QueueTestSuite, TestSynchronizedQueue_UniquePtr) {
    SynchronizedQueue<std::unique_ptr<X>> queue;
    ASSERT_EQ(0, queue.Size());

    queue.Add(std::unique_ptr<X>(new X(10)));
    queue.Add(std::unique_ptr<X>(new X(20)));
    queue.Add(std::unique_ptr<X>(new X(30)));
    ASSERT_EQ(3, queue.Size());

    ASSERT_EQ(10, queue.Poll()->id);
    ASSERT_EQ(2, queue.Size());

    ASSERT_EQ(20, queue.Poll()->id);
    ASSERT_EQ(1, queue.Size());

    ASSERT_EQ(30, queue.Poll()->id);
    ASSERT_EQ(0, queue.Size());
  }

  TEST(QueueTestSuite, TestBlockingQueue_Primitive) {
    BlockingQueue<int> queue;

    ASSERT_EQ(0, queue.Size());

    queue.Add(10);
    queue.Add(20);
    queue.Add(30);

    ASSERT_EQ(3, queue.Size());

    ASSERT_EQ(10, queue.Poll());
    ASSERT_EQ(2, queue.Size());

    ASSERT_EQ(20, queue.Poll());
    ASSERT_EQ(1, queue.Size());

    ASSERT_EQ(30, queue.Poll());
    ASSERT_EQ(0, queue.Size());
  }


  TEST(QueueTestSuite, TestBlockingQueue_UniquePtr) {
    BlockingQueue<std::unique_ptr<X>> queue;
    ASSERT_EQ(0, queue.Size());

    queue.Add(std::unique_ptr<X>(new X(10)));
    queue.Add(std::unique_ptr<X>(new X(20)));
    queue.Add(std::unique_ptr<X>(new X(30)));
    ASSERT_EQ(3, queue.Size());

    ASSERT_EQ(10, queue.Poll()->id);
    ASSERT_EQ(2, queue.Size());

    ASSERT_EQ(20, queue.Poll()->id);
    ASSERT_EQ(1, queue.Size());

    ASSERT_EQ(30, queue.Poll()->id);
    ASSERT_EQ(0, queue.Size());
  }
} // queuetest
} // concurrent
} // mdl

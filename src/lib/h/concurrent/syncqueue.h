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

#ifndef _MDL_CONCURRENT_QUEUE
#define _MDL_CONCURRENT_QUEUE

#include <list>
#include <memory>
#include <utility>

#include "../util/exception.h"
#include "synchronizable.h"
#include "semaphore.h"

namespace mdl {
namespace concurrent {

  template<class R>
  class Queue {
    public:
      // Default Constructible, Default Moveable, Copy Assignable, Move Assignable

      void Add(const R& item) {
        data.push_back(item);
      }

      void Add(R&& item) {
        data.push_back(std::move(item));
      }

      R Poll() {
        if (data.empty()) {
          throw util::not_found_exception("Cannot poll empty queue.");
        }
        R val = std::move(data.front());
        data.pop_front();
        return val;
      }

      int Size() {
        return data.size();
      }

    private:
      std::list<R> data;
  };

  template<class R>
  class SynchronizedQueue {
    public:
      SynchronizedQueue() {}

      // copyable, not movable.
      SynchronizedQueue(SynchronizedQueue& other) {
        other.sync.template Synchronized<void>([this, &other]() {
          queue = other.queue;
        });
      }

      // not assignable
      SynchronizedQueue& operator=(const SynchronizedQueue& other) = delete;
      SynchronizedQueue& operator=(SynchronizedQueue&& other) = delete;

      void Add(const R& item) {
        sync.Synchronized<void>([this, &item] () {
          queue.Add(item);
        });
      }

      void Add(R&& item) {
        sync.Synchronized<void>([this, &item] () {
          queue.Add(std::move(item));
        });
      }

      R Poll() {
        return sync.Synchronized<R>([this] () {
          return queue.Poll();
        });
      }

      int Size() {
        return sync.Synchronized<int>([this] () {
          return queue.Size();
        });
      }
    private:
      Queue<R> queue;
      mdl::concurrent::Synchronizable sync;
  };

  template<class R>
  class BlockingQueue  {
    public:
      BlockingQueue() {}
      // TODO: Consider making this copy constructible. Requires removing constness.
      BlockingQueue(const BlockingQueue& other) = delete;
      BlockingQueue(BlockingQueue&& other) = delete;

      BlockingQueue& operator=(const BlockingQueue& other) = delete;
      BlockingQueue& operator=(BlockingQueue&& other) = delete;

      void Add(const R& item) {
        semaphore.Up<void>([this, &item] (int numTickets) {
          queue.Add(item);
        });
      }

      void Add(R&& item) {
        semaphore.Up<void>([this, &item] (int numTickets) {
          queue.Add(std::move(item));
        });
      }

      R Poll() {
        return semaphore.Down<R>([this] (int numTickets) {
          return queue.Poll();
        });
      }

      int Size() const {
        return semaphore.NumTickets();
      }

      void InterruptAll() {
        semaphore.InterruptAll();
      }
    private:
      Queue<R> queue;
      mdl::concurrent::Semaphore semaphore;
  };

} // concurrent
} // mdl

#endif // _MDL_CONCURRENT_QUEUE

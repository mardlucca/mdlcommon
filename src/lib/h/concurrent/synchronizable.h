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

#ifndef _MDL_CONCURRENT_SYNCHRONIZABLE
#define _MDL_CONCURRENT_SYNCHRONIZABLE

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "../concurrent/threadlocal.h"

namespace mdl {
namespace concurrent {

  class Synchronizable {
    public: 
      typedef std::unique_lock<std::mutex> lock_t;

      Synchronizable();
      Synchronizable(const Synchronizable& other);
      Synchronizable(Synchronizable&& other) = delete;
      virtual ~Synchronizable();

      Synchronizable& operator=(const Synchronizable& other);
      Synchronizable& operator=(Synchronizable&& other) = delete;

      template<class T>
      T Synchronized(std::function<T ()> operation) {
        if (!threadLock) {
          auto guard = threadLock.Set(new lock_t(mutex));
          return operation();
        }
        
        return operation();
      }

      // template<class T>
      // T Synchronized(std::function<T ()> operation) const {
      //   if (!threadLock) {
      //     auto guard = threadLock.Set(new lock_t(mutex));
      //     return operation();
      //   }
        
      //   return operation();
      // }

      void Wait();
      void Notify();
      void NotifyAll();
      void Interrupt();

    private:
      std::atomic_long interruptedSeq;
      std::mutex mutex;
      std::condition_variable condition;
      mdl::concurrent::ThreadLocal<lock_t> threadLock;
  };

} // concurrent
} // mdl

#endif // _MDL_CONCURRENT_SYNCHRONIZABLE

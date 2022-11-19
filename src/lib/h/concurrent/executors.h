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

#ifndef _MDL_CONCURRENT_EXECUTOR
#define _MDL_CONCURRENT_EXECUTOR

#include <atomic>
#include <functional>

#include "exception.h"
#include "future.hpp"
#include "syncqueue.hpp"
#include "thread.h"

namespace mdl {
namespace concurrent {

  class ExecutorService {
    public:
      ExecutorService(int numThreads, ThreadFactory& threadFactory);
      ExecutorService(const ExecutorService& other) = delete;
      ExecutorService(ExecutorService&& other) = delete;
      virtual ~ExecutorService();
      ExecutorService& operator=(const ExecutorService& other) = delete;
      ExecutorService& operator=(ExecutorService&& other) = delete;

      void Execute(std::function<void ()> task);

      template <class T>
      Future<T> Submit(std::function<T ()> task);

      void Shutdown();
    private:
      BlockingQueue<std::function<void ()>> queue;
      std::list<std::thread> threads;
      std::atomic_bool shutdown = false;
      int numThreads;
      std::atomic_int numStoppedThreads = 0;

      void WorkerThreadFn();
      void ThreadInterrupterFn();
  };
  

  template <class T>
  Future<T> ExecutorService::Submit(std::function<T ()> task) {
    Future<T> future;
    queue.Add([future, task]() mutable {
      if (future.IsCanceled()) { return; }
      
      try {
        future.Set(task());
      } catch (int errorCode) {
        future.SetError("Failed to execute task", errorCode);
      } catch (const char * msg) {
        future.SetError(std::string(msg), -1);
      } catch (const std::string& msg) {
        future.SetError(msg, -1);
      } catch (const execution_exception& ex) {
        future.SetError(ex.what(), ex.what_code());
      } catch (const std::exception& ex) {
        future.SetError(ex.what(), -1);
      } catch (...) {
        future.SetError("Failed to execute task", -1);
      }
    });
    return future;
  }

} // concurrent
} // mdl

#endif // _MDL_CONCURRENT_EXECUTOR

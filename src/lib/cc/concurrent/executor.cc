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

#include "../../h/concurrent/executors.h"

#include <iostream>

namespace mdl {
namespace concurrent {
  ExecutorService::ExecutorService(int numThreads, ThreadFactory& threadFactory) 
      : numThreads(numThreads) {
    for (int i = 0; i < numThreads; i++) {
      threads.push_back(threadFactory.NewThread(&ExecutorService::WorkerThreadFn, this));
    }
  }

  ExecutorService::~ExecutorService() {
    Shutdown();
  }

  void ExecutorService::Execute(const std::function<void ()>& task) {
    queue.Add([task] () {
      try {
        task();
      } catch (...) {}
    });
  }

  void ExecutorService::Shutdown() {
    if (shutdown) { return; }

    shutdown = true;
    std::thread stopper(&ExecutorService::ThreadInterrupterFn, this);

    for (auto it = threads.begin(); it != threads.end(); it++) {
      it->join();
    }

    stopper.join();
  }

  void ExecutorService::WorkerThreadFn() {
    std::function<void ()> fn;

    while (!shutdown) {
      try {
        fn = queue.Poll();
      } catch (mdl::concurrent::interrupted_exception& ex) {
        break;
      }

      try {
        fn();
      } catch (std::exception& ex) {
        // TODO: What do we do here. Log, once logging supported.
      } catch (...) {}
    }

    numStoppedThreads++;
  }

  void ExecutorService::ThreadInterrupterFn() {
    const int delay = 100;

    while (numStoppedThreads.load() < numThreads) {
      queue.InterruptAll();
      this_thread::sleep(delay);
    }
  }

} // concurrent
} // mdl

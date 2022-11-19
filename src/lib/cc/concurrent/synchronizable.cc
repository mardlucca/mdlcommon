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

#include "../../h/concurrent/synchronizable.h"

#include <exception>

#include "../../h/concurrent/exception.h"

namespace mdl {
namespace concurrent {

  Synchronizable::Synchronizable() {}

  // This object keeps its own mutex, lock thread local and interruptedSeq
  Synchronizable::Synchronizable(const Synchronizable& other) {}

  Synchronizable::~Synchronizable() {}

  Synchronizable& Synchronizable::operator=(const Synchronizable& other) {
    // This object keeps its own mutex and lock thread local
    return *this;
  }

  void Synchronizable::Wait() {
    if (!threadLock) {
      throw std::runtime_error("Call to Wait while not syncrhonized.");
    }

    // we're locked.
    long seq = interruptedSeq.load();

    condition.wait(*threadLock);
    
    if (interruptedSeq.load() > seq) {
      throw interrupted_exception("Synchronizable interrupted by caller");
    }
  }

  void Synchronizable::Notify() {
    // While not stricly required in C++, will notify only when thread holds lock
    if (!threadLock) {
      throw std::runtime_error("Call to Notify while not syncrhonized.");
    }

    condition.notify_one();
  }

  void Synchronizable::NotifyAll() {
    // While not stricly required in C++, will notify only when thread holds lock
    if (!threadLock) {
      throw std::runtime_error("Call to Notify while not syncrhonized.");
    }

    condition.notify_all();
  }

  void Synchronizable::Interrupt() {
    if (!threadLock) {
      throw std::runtime_error("Call to Notify while not syncrhonized.");
    }

    interruptedSeq++;
    condition.notify_all();
  }

} // concurrent
} // mdl

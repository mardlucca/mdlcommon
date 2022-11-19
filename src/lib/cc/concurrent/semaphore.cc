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

#include "../../h/concurrent/semaphore.h"

namespace mdl {
namespace concurrent {

  Semaphore::Semaphore(long tickets) : tickets(tickets) {}

  void Semaphore::Up() {
    sync.Synchronized<void>([this]() {
      tickets++;
      sync.Notify();
    });
  }

  template<>
  void Semaphore::Up<void>(std::function<void (long)> doBeforeFn) {
    return sync.Synchronized<void>([this, &doBeforeFn]() {
      doBeforeFn(tickets);
      tickets++;
      sync.Notify();
    });
  }

  void Semaphore::Down() {
    sync.Synchronized<void>([this]() {
      tickets--;
      if (tickets < 0) {
        sync.Wait();
      }
    });
  }

  template<>
  void Semaphore::Down<void> (std::function<void (long)> doAfterFn) {
     sync.Synchronized<void>([this, &doAfterFn]() {
      tickets--;
      if (tickets < 0) {
        sync.Wait();
      }
      doAfterFn(tickets);
    });
  }

  long Semaphore::NumTickets() const {
    return tickets.load();
  }

  void Semaphore::InterruptAll() {
    sync.Synchronized<void>([this]() {
      // all interrupted threads will be awaken, and they will not require a ticket anymore. For 
      //  that reason, we set the number of tickets back to zero.
      if (tickets.load() < 0) { tickets.store(0); }
      
      sync.Interrupt();
    });
  }

} // concurrent
} // mdl

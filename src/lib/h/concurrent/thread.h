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

#ifndef _MDL_CONCURRENT_THREAD
#define _MDL_CONCURRENT_THREAD

#include <atomic>
#include <string>
#include <thread>

#include "syncqueue.hpp"

#include <iostream>

namespace mdl {
namespace concurrent {
  namespace this_thread {
    extern thread_local std::string name;
    const std::string& get_name();

    std::thread::id get_id() noexcept;

    inline void sleep(int timeMillis) noexcept {
      std::this_thread::sleep_for(std::chrono::duration<long, std::milli>(timeMillis));
    }
  }

  typedef BlockingQueue<std::function<void ()>> worker_queue_t;

  template<class Function, class... Args>
  void _named_thread_fn(const std::string name, Function&& f, Args&&... args) {
    this_thread::name = name;
    try {
      std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
    } catch (...) {
      this_thread::name = "";
      throw;
    }
  }

  template<class Function, class... Args>
  std::thread named_thread(const std::string& name, Function& f, Args&&... args) {
    // if f is an lvalue reference, must use std::ref since decay behavior in std::thread removes
    //  reference otherwise. Forwarding, in this case, wouldn't work as the forwarded type would
    //  stil be an lvalue ref..
    // Also, the reason why std::thread uses decay is because it wants to copy the values from the
    //  function/arguments before spawning the new thread. std::__decay_copy uses the type's copy
    //  constructor or move constructor, depending on the active semantics (move/copy)
    return std::thread(_named_thread_fn<Function, Args...>,
        name,
        std::ref(f),
        std::forward<Args>(args)...);
  }

  template<class Function, class... Args>
  std::thread named_thread(const std::string& name, Function&& f, Args&&... args) {
    // f is guaranteed to be an rvalue ref, so moving suffices (i.e. std::thread will receive an
    //   rvalue ref for f, as it would if the caller was calling it directly). Args, on the other
    //   hand, needs forwarding because individual args may be lvalue refs.
    return std::thread(_named_thread_fn<Function, Args...>,
        name,
        std::move(f),
        std::forward<Args>(args)...);
  }

  class ThreadFactory {
    public:
      ThreadFactory(const std::string& name);

      template <class Function, class... Args>
      std::thread NewThread(Function&& f, Args&&... args);
    private:
      std::string name;
      std::atomic_int idSeq;

      std::string NextName();
  };

  template <class Function, class... Args>
  std::thread ThreadFactory::NewThread(Function&& f, Args&&... args) {
    // std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
    return named_thread(
        NextName(), 
        std::forward<Function>(f),
        std::forward<Args>(args)...);
  }

} // concurrent
} // mdl

#endif // _MDL_CONCURRENT_THREAD

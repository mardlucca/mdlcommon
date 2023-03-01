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

#ifndef _MDL_CONCURRENT_THREAD_LOCAL
#define _MDL_CONCURRENT_THREAD_LOCAL

#include <atomic>
#include <type_traits>
#include <unordered_map>

namespace mdl {
namespace concurrent {
  extern thread_local std::unordered_map<long, void*> _thread_local_vars;
  extern std::atomic_long _thread_local_id_seq;
  
  template <class T>
  class ThreadLocal {
    public: 
      typedef typename std::remove_extent<T>::type valueType;
      typedef typename std::add_pointer<valueType>::type pointer;
      typedef typename std::add_lvalue_reference<valueType>::type reference;

      class Guard {
        public:
          Guard(long id) : id(id) {}
          Guard(const Guard& other) = delete;
          Guard(Guard&& other) = delete;
          Guard& operator=(const Guard& other) = delete;
          Guard& operator=(Guard&& other) = delete;
          ~Guard() {
            ThreadLocal<T>::Remove(id); 
          }
        private:
          long id;
      };

      ThreadLocal() : id(++_thread_local_id_seq) {}

      ThreadLocal(const ThreadLocal<T>& other) = delete;

      ThreadLocal(ThreadLocal<T>&& other) = delete;

      // ~ThreadLocal() {}

      ThreadLocal<T>& operator=(const ThreadLocal<T>& other) = delete;

      ThreadLocal<T>& operator=(ThreadLocal<T>&& other) = delete;

      reference operator*() {
        return *Get();
      }

      reference operator[](std::size_t index) {
        return Get()[index];
      }

      const reference operator*() const {
        return *Get();
      }

      pointer operator->() {
        return Get();
      }

      const pointer operator->() const {
        return Get();
      }

      operator bool() const {
        return Get(id) != nullptr;
      }

      pointer Get() {
        return Get(id);
      }

      const pointer Get() const {
        return _thread_local_vars.count(id)
            ? static_cast<pointer>(_thread_local_vars[id])
            : nullptr;
      }

      Guard Set(pointer value) {
        if (_thread_local_vars.count(id)) {
          pointer current = static_cast<pointer>(_thread_local_vars[id]);
          _thread_local_vars[id] = value;
          if (current) {
            if (std::is_array<T>::value) {
              delete[] current; 
            } else {
              delete current; 
            }
          }
          return Guard(0);    // no-op guard, this is not the first call to a bind value to thread.
        }

        // no values bound yet, so this must return a "hot" guard.
        _thread_local_vars[id] = value;
        return Guard(id);     // returns a hot guard (will actually Remove when destroyed)
      }

    private:
      long id;

      static pointer Get(int id) {
        return _thread_local_vars.count(id)
            ? static_cast<pointer>(_thread_local_vars[id])
            : nullptr;
      }

      static void Remove(long id) {
        pointer current = Get(id);
        _thread_local_vars.erase(id);
        if (current) { 
          if (std::is_array<T>::value) {
            delete[] current; 
          } else {
            delete current; 
          }
        }
      }
  };


} // concurrent
} // mdl

#endif // _MDL_CONCURRENT_THREAD_LOCAL

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

#ifndef _MDL_CONCURRENT_FUTURE
#define _MDL_CONCURRENT_FUTURE

#include <functional>
#include <memory>

#include "exception.h"
#include "synchronizable.h"

namespace mdl {
namespace concurrent {
  template <class T>
  class Future {
    public:
      typedef std::decay_t<T> valueType;

      Future();
      Future(const Future<T>& other);
      Future(Future<T>&& other);
      virtual ~Future();
      Future<T>& operator=(const Future<T>& other);
      Future<T>& operator=(Future<T>&& other);

      valueType Get();
      bool Cancel();
      bool IsCanceled();
      bool IsDone();

    protected:
      virtual bool Start();
      virtual void Set(T&& value);
      virtual void SetError(const std::string& errorMsg, int errorCode);

    private:
      struct Data {
        valueType value;
        int errorCode;
        std::string errorMsg;

        bool started;
        bool cancelled;
        bool done;
        bool failed;
        Synchronizable sync;
      };

      std::shared_ptr<Data> data;

      friend class ExecutorService;
  };


  template <class T>
  Future<T>::Future() : data(new Data()) {}

  template <class T>
  Future<T>::Future(const Future<T>& other) : data(other.data) {}

  template <class T>
  Future<T>::Future(Future<T>&& other) : data(std::move(other.data)) {}

  template <class T>
  Future<T>::~Future() {}

  template <class T>
  Future<T>& Future<T>::operator=(const Future<T>& other) {
    data = other.data;
  }

  template <class T>
  Future<T>& Future<T>::operator=(Future<T>&& other) {
    data = std::move(other.data);
  }

  template <class T>
  typename Future<T>::valueType Future<T>::Get() {
    return data->sync.template Synchronized<valueType>([this]() {
      while (!data->done) {
        data->sync.Wait();
      }

      if (data->cancelled) {
        throw interrupted_exception("Task has been cancelled");
      }
      if (data->failed) {
        throw execution_exception(data->errorMsg, data->errorCode);
      }

      return data->value;
    });
  }

  template <class T>
  bool Future<T>::Cancel() {
    return data->sync.template Synchronized<bool>([this]() {
      if (data->cancelled) { return true; }
      if (data->started || data->done) { return false; }

      data->cancelled = true;
      data->done = true;

      data->sync.NotifyAll();
      return true;
    });
  }

  template <class T>
  bool Future<T>::IsCanceled() {
    return data->sync.template Synchronized<bool>([this]() {
      return data->cancelled;
    });
  }
    
  template <class T>
  bool Future<T>::IsDone() {
    return data->sync.template Synchronized<bool>([this]() {
      return data->done;
    });
  }

  template <class T>
  bool Future<T>::Start() {
    return data->sync.template Synchronized<bool>([this]() {
      // should not start task that's already done.
      if (data->done) { return false; }

      data->started = true;
      return true;
    });
  }

  template <class T>
  void Future<T>::Set(T&& value) {
    data->sync.template Synchronized<void>([this, &value]() {
      if (data->done) { return; }

      data->value = std::forward<T>(value);
      data->done = true;
      data->sync.NotifyAll();
    });
  }

  template <class T>
  void Future<T>::SetError(const std::string& errorMsg, int errorCode) {
    data->sync.template Synchronized<void>([this, errorMsg, errorCode]() {
      if (data->done) { return; }

      data->done = true;
      data->failed = true;
      data->errorCode = errorCode;
      data->errorMsg = errorMsg;
      data->sync.NotifyAll();
    });
  }
  
} // concurrent
} // mdl

#endif // _MDL_CONCURRENT_FUTURE

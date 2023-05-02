// Copyright (c) 2023, Marcio Lucca
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

#ifndef _MDL_UTIL_FUNCTIONAL
#define _MDL_UTIL_FUNCTIONAL

#include <functional>
#include <iterator>
#include <iostream>
namespace mdl {
namespace util {
namespace functional {

  bool IsEven(int val);
  bool IsOdd(int val);
  int Negate(int val);

  /**
   * Converts from funtion pointer to std::function. At times the compiler has trouble deducing
   * types parameters when converting from function pointer types to std::function. This servers
   * as a hint to help.
  */
  template <class T, class F>
  inline std::function<T (const F&)> AsFunction(T (* arg)(const F&)) {
    return arg;
  }

  template <class T, class F>
  inline std::function<T (F*)> AsFunction(T (* arg)(F*)) {
    return arg;
  }

  template <class T>
  std::function<void ()> Set(T* ptr, const T& val) {
    return [ptr, val]() {
      *ptr = val;
    };
  }

  template <class T>
  std::function<void (T)> Assign(T* ptr) {
    return [ptr](const T& val) {
      *ptr = val;
    };
  }

  template <class T, class F>
  std::function<void (const F&)> Assign(T* out, const std::function<T (const F&)>& castFn) {
    return [out, &castFn](const F& val) {
      *out = castFn(val);
    };
  }


  template <class T>
  class SupplierIterator 
      : public std::iterator<std::input_iterator_tag, T> {
    public:
      SupplierIterator(const std::function<T* ()>& supplier) 
          : supplier(supplier), currVal(this->supplier()) {}
      SupplierIterator(T* currVal = nullptr) : currVal(currVal) {}

      SupplierIterator<T>& operator++() {
        currVal = supplier();
        return *this;
      }
      SupplierIterator<T> operator++(int) {
        T* val = currVal;
        currVal = supplier();
        return SupplierIterator<T>(val);
      }
      bool operator==(const SupplierIterator<T>& other) const {
        return currVal == other.currVal;
      }
      bool operator!=(const SupplierIterator<T>& other) const {
        return currVal != other.currVal;
      }
      T& operator*() const { return *currVal; }
      T* operator->() const { return currVal; }

    private:
      std::function<T* ()> supplier;
      T* currVal;
  };

  template <class T>
  class SupplierIterable {
    public:
      SupplierIterable(const std::function<T* ()>& supplier) 
          : beginIterator(supplier), endIterator(SupplierIterator<T>()) {}
      SupplierIterator<T>& begin() { return beginIterator; }
      SupplierIterator<T> end() { return endIterator; }
    private:
      SupplierIterator<T> beginIterator;
      SupplierIterator<T> endIterator;
  };

  template <class T>
  std::function<T* ()> AsSupplier(const std::function<std::unique_ptr<T> ()>& supplier) {
    return [supplier, val = std::shared_ptr<T>()] () mutable {
      val = supplier();
      return val.get();
    };
  }

  template <class T>
  std::function<T* ()> AsSupplier(T* value) {
    return [supplied = false, value] () mutable {
      T* result = supplied ? nullptr : value;
      supplied = true;
      return result;
    };
  }

} // namespace functional
} // namespace util
} // namespace mdl

#endif // _MDL_UTIL_FUNCTIONAL
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

#include "mdl/concurrent.h"

#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <unordered_map>

using std::cout;
using std::endl;

namespace mdl {
namespace concurrent {
namespace template_args_test {

  void someFn(int n) {}

  template<class T>
  void foo(const T& arg) {
    cout << "foo: " << arg << endl;
    cout << "Is type: " << std::is_same<int, T>::value << endl;
    cout << typeid(decltype(arg)).name() << endl;
    cout << "Is rvalue : " << std::is_rvalue_reference<T>::value << endl;
    cout << "Is lvalue : " << std::is_lvalue_reference<T>::value << endl;
    cout << "Is rvalue &: " << std::is_rvalue_reference<T&>::value << endl;
    cout << "Is lvalue &: " << std::is_lvalue_reference<T&>::value << endl;
    cout << "Is rvalue &&: " << std::is_rvalue_reference<T&&>::value << endl;
    cout << "Is lvalue &&: " << std::is_lvalue_reference<T&&>::value << endl;
    cout << "Is rvalue arg: " << std::is_rvalue_reference<decltype(arg)>::value << endl;
    cout << "Is lvalue arg: " << std::is_lvalue_reference<decltype(arg)>::value << endl;
    cout << "Is const arg: " << std::is_const<typename std::remove_reference<decltype(arg)>::type>::value << endl;
    cout << "Is function: " << std::is_function<T>::value << endl;
    cout << "Is pointer: " << std::is_pointer<T>::value << endl;
  }

  template<class T>
  void foo2(T&& arg) {
    cout << "foo: " << arg << endl;
    cout << "Is type: " << std::is_same<int, T>::value << endl;
    cout << typeid(decltype(arg)).name() << endl;
    cout << "Is rvalue : " << std::is_rvalue_reference<T>::value << endl;
    cout << "Is lvalue : " << std::is_lvalue_reference<T>::value << endl;
    cout << "Is rvalue &: " << std::is_rvalue_reference<T&>::value << endl;
    cout << "Is lvalue &: " << std::is_lvalue_reference<T&>::value << endl;
    cout << "Is rvalue &&: " << std::is_rvalue_reference<T&&>::value << endl;
    cout << "Is lvalue &&: " << std::is_lvalue_reference<T&&>::value << endl;
    cout << "Is rvalue arg: " << std::is_rvalue_reference<decltype(arg)>::value << endl;
    cout << "Is lvalue arg: " << std::is_lvalue_reference<decltype(arg)>::value << endl;
    cout << "Is const arg: " << std::is_const<typename std::remove_reference<decltype(arg)>::type>::value << endl;
    cout << "Is function: " << std::is_function<T>::value << endl;
    cout << "Is pointer: " << std::is_pointer<T>::value << endl;
    cout << "arg == T: " << std::is_same<T, decltype(arg)>::value << endl;
  }

  template<class T>
  void foo3(T arg) {
    cout << "foo: " << arg << endl;
    cout << "Is type: " << std::is_same<int, T>::value << endl;
    cout << typeid(decltype(arg)).name() << endl;
    cout << "Is rvalue : " << std::is_rvalue_reference<T>::value << endl;
    cout << "Is lvalue : " << std::is_lvalue_reference<T>::value << endl;
    cout << "Is rvalue &: " << std::is_rvalue_reference<T&>::value << endl;
    cout << "Is lvalue &: " << std::is_lvalue_reference<T&>::value << endl;
    cout << "Is rvalue &&: " << std::is_rvalue_reference<T&&>::value << endl;
    cout << "Is lvalue &&: " << std::is_lvalue_reference<T&&>::value << endl;
    cout << "Is rvalue arg: " << std::is_rvalue_reference<decltype(arg)>::value << endl;
    cout << "Is lvalue arg: " << std::is_lvalue_reference<decltype(arg)>::value << endl;
    cout << "Is const arg: " << std::is_const<typename std::remove_reference<decltype(arg)>::type>::value << endl;
    cout << "Is function: " << std::is_function<T>::value << endl;
    cout << "Is pointer: " << std::is_pointer<T>::value << endl;
    cout << "arg == T: " << std::is_same<T, decltype(arg)>::value << endl;
  }


  // TEST(TemplateArgsTestSuite, Test_Me) {
  //   cout << "---- [1] -----" << endl;
  //   foo(1);
  //   cout << "----" << endl;
  //   foo2(1);
  //   cout << "----" << endl;
  //   foo3(1);

  //   int x = 2;
  //   cout << "\n---- [x] -----" << endl;
  //   foo(x);
  //   cout << "----" << endl;
  //   foo2(x);
  //   cout << "----" << endl;
  //   foo3(x);

  //   cout << "\n---- [&x] -----" << endl;
  //   foo(&x);
  //   cout << "----" << endl;
  //   foo2(&x);
  //   cout << "----" << endl;
  //   foo3(&x);

  //   int* y = &x;
  //   cout << "\n---- [y (i.e. int *)] -----" << endl;
  //   foo(y);
  //   cout << "----" << endl;
  //   foo2(y);
  //   cout << "----" << endl;
  //   foo3(y);

  //   int& z = x;
  //   cout << "\n---- [z (i.e. int &)] -----" << endl;
  //   foo(z);
  //   cout << "----" << endl;
  //   foo2(z);
  //   cout << "----" << endl;
  //   foo3(z);

  //   cout << "\n---- [someFn] -----" << endl;
  //   foo(someFn);
  //   cout << "----" << endl;
  //   foo2(someFn);
  //   cout << "----" << endl;
  //   foo3(someFn);

  //   cout << "\n---- [&someFn] -----" << endl;
  //   foo(&someFn);
  //   cout << "----" << endl;
  //   foo2(&someFn);
  //   cout << "----" << endl;
  //   foo3(&someFn);

  //   cout << "\n---- [inline lambda] -----" << endl;
  //   foo([]() {return 10;});
  //   cout << "----" << endl;
  //   foo2([]() {return 10;});
  //   cout << "----" << endl;
  //   foo3([]() {return 10;});

  //   cout << "\n---- [lambda var] -----" << endl;
  //   auto fn = []() {return 10;};
  //   foo(fn);
  //   cout << "----" << endl;
  //   foo2(fn);
  //   cout << "----" << endl;
  //   foo3(fn);

  //   cout << "\n---- std::move(x) -----" << endl;
  //   foo(std::move(x));
  //   cout << "----" << endl;
  //   foo2(std::move(x));
  //   cout << "----" << endl;
  //   foo3(std::move(x));
  // }

  template <class T>
  void bar(T&& val) {
    foo<T&&>(val);
  }

  template <class T>
  void bar2(T&& val) {
    foo2<T&&>(std::forward<T>(val));
  }

  template <class T>
  void bar3(T&& val) {
    foo3<T&&>(std::forward<T>(val));
  }


  // TEST(TemplateArgsTestSuite, Test_Me2) {
  //   cout << "\n---- 1 -----" << endl;
  //   bar(1);
  //   cout << "----" << endl;
  //   bar2(1);
  //   cout << "----" << endl;
  //   bar3(1);

  //   int x = 2;
  //   cout << "\n---- [x] -----" << endl;
  //   bar(x);
  //   cout << "----" << endl;
  //   bar2(x);
  //   cout << "----" << endl;
  //   bar3(x);
  // }

} // threadtest
} // concurrent
} // mdl

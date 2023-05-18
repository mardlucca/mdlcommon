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

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#include "mdl/util.h"
#include <mdl/text.h>

using std::cout;
using std::endl;

namespace mdl {
namespace util {
namespace functional {

  TEST(FunctionalTestSuite, TestSetting) {
    int x = 10;
    auto settingFn = Assign(&x, 20);
    settingFn();
    ASSERT_EQ(20, x);
  }

  TEST(FunctionalTestSuite, TestAssigning) {
    int x = 10;
    auto assigningFn = Assign(&x);
    assigningFn(20);
    ASSERT_EQ(20, x);
  }

  TEST(FunctionalTestSuite, TestCastingAndAssigning) {
    int x = 10;
    auto assigningFn = Assign(&x, AsFunction(mdl::text::ParseInt));
    assigningFn("20");
    ASSERT_EQ(20, x);
  } 

  TEST(FunctionalTestSuite, TestSupplierIterable) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    int i = 0;
    std::function<int*()> supplier = [&vec, &i]() {
      if (i < vec.size()) {
        return vec.data() + i++;
      }
      return static_cast<int *>(nullptr);
    };

    for (auto& val : SupplierIterable(supplier)) {
      ASSERT_EQ(i, val);
      val += 10;
    }
    i = 0;
    for (auto val : SupplierIterable(supplier)) {
      ASSERT_EQ(10 + i, val);
      val -= 10;
    }
    i = 0;
    for (auto val : SupplierIterable(supplier)) {
      ASSERT_EQ(10 + i, val);
    }
  }

  TEST(FunctionalTestSuite, TestReadOnlySupplierIterable) {
    std::vector<int> vec = {11, 12, 13, 14, 15};
    int i = 0;
    std::function<const int*()> supplier = [&vec, &i]() {
      if (i < vec.size()) {
        return static_cast<const int *>(vec.data() + i++);
      }
      return static_cast<const int *>(nullptr);
    };

    int expected = 11;
    for (const int& val : SupplierIterable(supplier)) {
      ASSERT_EQ(expected++, val);
    }
  }

  TEST(FunctionalTestSuite, TestPointerAsSupplier) {
    int x = 10;
    auto supplier = AsSupplier(&x);

    ASSERT_EQ(x, *supplier());
    ASSERT_EQ(nullptr, supplier());
    ASSERT_EQ(nullptr, supplier());
  }
} // namespace functional
} // namespace util
} // namespace mdl
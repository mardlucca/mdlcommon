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

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>


#include "mdl/io.h"
#include "mdl/text.h"

namespace mdl {
namespace text {

  TEST(NumParseTestSuite, TestParseDouble) {
    ASSERT_EQ(1.2345, mdl::text::ParseDouble("1.2345"));
    ASSERT_EQ(1.23, mdl::text::ParseDouble(L"1.23"));
  }

  TEST(NumParseTestSuite, TestParseDouble_Invalid) {
    ASSERT_THROW(mdl::text::ParseDouble("az1"), mdl::text::parse_exception);
  }

  TEST(NumParseTestSuite, TestParseInt) {
    ASSERT_EQ(12, mdl::text::ParseInt(L"12"));
    ASSERT_EQ(13, mdl::text::ParseInt(L" 13"));
    ASSERT_EQ(14, mdl::text::ParseInt(L"14 "));

    ASSERT_EQ(15, mdl::text::ParseInt(" 15 "));
  }

  TEST(NumParseTestSuite, TestParseInt_Invalid) {
    ASSERT_THROW(mdl::text::ParseInt(L"z12"), mdl::text::parse_exception);
  }

} // namespace text
} // namespace mdl
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

#include "mdl/util.h"

TEST(StringTestSuite, TestSplitChar) {
  std::vector<std::string> str = mdl::util::string::Split("one,two,three");
  ASSERT_EQ(3, str.size());
  ASSERT_EQ("one", str[0]);
  ASSERT_EQ("two", str[1]);
  ASSERT_EQ("three", str[2]);
}

TEST(StringTestSuite, TestSplitChar_Semicolon) {
  std::vector<std::string> str = mdl::util::string::Split("one;two;three", ';');
  ASSERT_EQ(3, str.size());
  ASSERT_EQ("one", str[0]);
  ASSERT_EQ("two", str[1]);
  ASSERT_EQ("three", str[2]);
}

TEST(StringTestSuite, TestSplitCharWide) {
  std::vector<std::wstring> str = mdl::util::string::Split(L"one,two,three");
  ASSERT_EQ(3, str.size());
  ASSERT_EQ(L"one", str[0]);
  ASSERT_EQ(L"two", str[1]);
  ASSERT_EQ(L"three", str[2]);
}

TEST(StringTestSuite, TestSplitCharWide_Semicolon) {
  std::vector<std::wstring> str = mdl::util::string::Split(L"one;two;three", L';');
  ASSERT_EQ(3, str.size());
  ASSERT_EQ(L"one", str[0]);
  ASSERT_EQ(L"two", str[1]);
  ASSERT_EQ(L"three", str[2]);
}

TEST(StringTestSuite, TestSplitChar_Empty1) {
  std::vector<std::string> str = mdl::util::string::Split(",two,three");
  ASSERT_EQ(3, str.size());
  ASSERT_EQ("", str[0]);
  ASSERT_EQ("two", str[1]);
  ASSERT_EQ("three", str[2]);
}

TEST(StringTestSuite, TestSplitChar_Empty2) {
  std::vector<std::string> str = mdl::util::string::Split("one,,three");
  ASSERT_EQ(3, str.size());
  ASSERT_EQ("one", str[0]);
  ASSERT_EQ("", str[1]);
  ASSERT_EQ("three", str[2]);
}

TEST(StringTestSuite, TestSplitChar_Empty3) {
  std::vector<std::string> str = mdl::util::string::Split("one,two,");
  ASSERT_EQ(3, str.size());
  ASSERT_EQ("one", str[0]);
  ASSERT_EQ("two", str[1]);
  ASSERT_EQ("", str[2]);
}

TEST(StringTestSuite, TestSplitChar_Empty4) {
  std::vector<std::string> str = mdl::util::string::Split(",,");
  ASSERT_EQ(3, str.size());
  ASSERT_EQ("", str[0]);
  ASSERT_EQ("", str[1]);
  ASSERT_EQ("", str[2]);
}

TEST(StringTestSuite, TestSplitChar_String) {
  std::string s = "one,two,three";
  std::vector<std::string> str = mdl::util::string::Split(s);
  ASSERT_EQ(3, str.size());
  ASSERT_EQ("one", str[0]);
  ASSERT_EQ("two", str[1]);
  ASSERT_EQ("three", str[2]);
}

TEST(StringTestSuite, TestSplitChar_WString) {
  std::wstring s = L"one,two,three";
  std::vector<std::wstring> str = mdl::util::string::Split(s);
  ASSERT_EQ(3, str.size());
  ASSERT_EQ(L"one", str[0]);
  ASSERT_EQ(L"two", str[1]);
  ASSERT_EQ(L"three", str[2]);
}

TEST(StringTestSuite, TestTrim) {
  ASSERT_EQ("blah", mdl::util::string::Trim("   blah   "));
  ASSERT_EQ("blah", mdl::util::string::Trim(" blah "));
  ASSERT_EQ("blah", mdl::util::string::Trim("blah   "));
  ASSERT_EQ("blah", mdl::util::string::Trim("   blah"));
  ASSERT_EQ("blah", mdl::util::string::Trim("blah"));

  ASSERT_EQ("a b c", mdl::util::string::Trim("   a b c "));
  ASSERT_EQ("a\nb\nc", mdl::util::string::Trim("\n\t a\nb\nc\n\t "));
  ASSERT_EQ("", mdl::util::string::Trim("\n\t \n\n\n\t "));
  ASSERT_EQ("", mdl::util::string::Trim(""));
}

TEST(StringTestSuite, TestTrim_WideChar) {
  ASSERT_EQ(L"blah", mdl::util::string::Trim(L"   blah   "));
  ASSERT_EQ(L"blah", mdl::util::string::Trim(L" blah "));
  ASSERT_EQ(L"blah", mdl::util::string::Trim(L"blah   "));
  ASSERT_EQ(L"blah", mdl::util::string::Trim(L"   blah"));
  ASSERT_EQ(L"blah", mdl::util::string::Trim(L"blah"));

  ASSERT_EQ(L"a b c", mdl::util::string::Trim(L"   a b c "));
  ASSERT_EQ(L"a\nb\nc", mdl::util::string::Trim(L"\n\t a\nb\nc\n\t "));
  ASSERT_EQ(L"", mdl::util::string::Trim(L"\n\t \n\n\n\t "));
  ASSERT_EQ(L"", mdl::util::string::Trim(L""));
}

TEST(StringTestSuite, TestTrim_String) {
  std::string s = "   blah   ";
  ASSERT_EQ("blah", mdl::util::string::Trim(s));
}

TEST(StringTestSuite, TestTrim_WideString) {
  std::wstring s = L"   blah   ";
  ASSERT_EQ(L"blah", mdl::util::string::Trim(s));
}
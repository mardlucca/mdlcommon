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
#include <vector>

#include <cstdlib>
#include <filesystem>

#include "mdl/io.h"
#include "mdl/text.h"

namespace mdl {
namespace text {

  enum class EventType { newLine, value, terminate };

  class TestListener : public ICsvParseListener {
    public:
      void OnNewLine() override {
        ASSERT_FALSE(finished);
        ASSERT_EQ(EventType::newLine, expectedEvent[expectedIndex]);
        expectedIndex++;
      }

      void OnValue(const std::string& val) override {
        ASSERT_FALSE(finished);
        ASSERT_EQ(EventType::value, expectedEvent[expectedIndex]);
        ASSERT_EQ(expectedValue[expectedIndex], val);
        expectedIndex++;
      }

      void OnTerminate() override {
        ASSERT_FALSE(finished);
        ASSERT_EQ(EventType::terminate, expectedEvent[expectedIndex]);
        expectedIndex++;
        finished = true;
      }

      void Expect(const char* value) {
        expectedEvent.push_back(EventType::value);
        expectedValue.push_back(value);
      }

      void ExpectNewLine() {
        expectedEvent.push_back(EventType::newLine);
        expectedValue.push_back(nullptr);
      }

      void ExpectTerminate() {
        expectedEvent.push_back(EventType::terminate);
        expectedValue.push_back(nullptr);
      }

      void ExpectFinished() {
        ASSERT_TRUE(finished);
      }
    private:
      std::vector<EventType> expectedEvent;
      std::vector<const char*> expectedValue;
      int expectedIndex = 0;
      bool finished = false;
  };

  TEST(CsvParseTestSuite, SingleValueTest_Unquoted) {
    TestListener listener;
    listener.Expect("te st");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("te st");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, SingleValueTest_Quoted) {
    TestListener listener;
    listener.Expect("te,st");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("\"te,st\"");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, DoubleValueTest_Unquoted) {
    TestListener listener;
    listener.Expect("test");
    listener.Expect("test2");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("test,test2");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, DoubleValueTest_Quoted) {
    TestListener listener;
    listener.Expect("test");
    listener.Expect("test2");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("\"test\",\"test2\"");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, DoubleValueTest_Mixed1) {
    TestListener listener;
    listener.Expect("test");
    listener.Expect("test2");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("\"test\",test2");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, DoubleValueTest_Mixed2) {
    TestListener listener;
    listener.Expect("test");
    listener.Expect("test2");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("test,\"test2\"");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, EmptyValueTest_Initial) {
    TestListener listener;
    listener.Expect("");
    listener.Expect("");
    listener.Expect("test");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str(",,test");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, EmptyValueTest_Middle) {
    TestListener listener;
    listener.Expect("test");
    listener.Expect("");
    listener.Expect("");
    listener.Expect("test2");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("test,,,test2");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, EmptyValueTest_End) {
    TestListener listener;
    listener.Expect("test");
    listener.Expect("");
    listener.Expect("");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("test,,");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, NewLineTest_AfterUnquoted) {
    TestListener listener;
    listener.Expect("test");
    listener.ExpectNewLine();
    listener.Expect("test2");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("\ntest\n\ntest2");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }
  
  TEST(CsvParseTestSuite, NewLineTest_AfterQuoted) {
    TestListener listener;
    listener.Expect("test");
    listener.ExpectNewLine();
    listener.Expect("test2");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("\n\"test\"\n\ntest2");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, NewLineTest_AfterEmpty) {
    TestListener listener;
    listener.Expect("");
    listener.Expect("");
    listener.ExpectNewLine();
    listener.Expect("test2");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("\n,\n\ntest2");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, MultilineValueTest) {
    TestListener listener;
    listener.Expect("a\nb");
    listener.Expect("c");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("\"a\nb\",c");
    CsvParse(in, listener);
    listener.ExpectFinished();
  }

  TEST(CsvParseTestSuite, UnterminatedQuotedValueTest) {
    TestListener listener;
    listener.Expect("a\nb");
    listener.Expect("c");
    listener.ExpectTerminate();

    std::istringstream in;
    in.str("\"a\nb,c");
    ASSERT_THROW(CsvParse(in, listener), mdl::text::parse_exception);
  }

  TEST(CsvParseTestSuite, UnterminatedQuotedValueTest_BeforeEOF) {
    TestListener listener;
    listener.Expect("");

    std::istringstream in;
    in.str(",\"abc");
    ASSERT_THROW(CsvParse(in, listener), mdl::text::parse_exception);
  }

  TEST(CsvParseTestSuite, InvalidCharAfterFinalQuoteTest) {
    TestListener listener;

    std::istringstream in;
    in.str("\"abc\"x,");
    ASSERT_THROW(CsvParse(in, listener), mdl::text::parse_exception);
  }  

  TEST(CsvParseTestSuite, FromFileTest) {
    CsvParse("src/test/resources/text/CsvParseTestSuite_FromFile.csv", 
        StdOutCsvParseListener::GetInstance());
  }

  TEST(CsvParseTestSuite, FromFileTest_NotFound) {
    ASSERT_THROW(CsvParse("bogus.csv", StdOutCsvParseListener::GetInstance()), 
        mdl::io::file_not_found_exception);
  }
} // namespace text
} // namespace mdl
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

#include "../../h/text/csvparse.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "../../h/io/exception.h"
#include "../../h/text/exception.h"
#include "../../h/util/exception.h"

namespace mdl {
namespace text {
  using mdl::util::raise;

  void ICsvParseListener::OnTerminate() {}

  StdOutCsvParseListener StdOutCsvParseListener::_instance;

  StdOutCsvParseListener::StdOutCsvParseListener(char quoteChar, char delimiterChar) 
      : count(0), quoteChar(quoteChar), delimiterChar(delimiterChar) {}

  void StdOutCsvParseListener::OnNewLine() {
    count = 0;
    std::cout << std::endl;
  };

  void StdOutCsvParseListener::OnValue(const std::string& val) {
    if (count > 0) {
      std::cout << delimiterChar;
    }

    std::cout << quoteChar << val << quoteChar;
    count++;
  };

  ICsvParseListener& StdOutCsvParseListener::GetInstance() {
    return _instance;
  }

  enum class CsvParseState {
    initial,
    reading,
    unquoted_reading,
    quoted_reading, 
    quoted_found_quote
  };

  CsvParseState CsvParseHandleInitial(
      char ch, 
      std::string& value,
      char quoteChar, 
      char separatorChr,
      ICsvParseListener& listener) {
    if (ch == '\n' || ch == -1) {
      // ignore blank lines
      return CsvParseState::initial; 
    }
    
    if (ch == quoteChar) { 
      return CsvParseState::quoted_reading; 
    }
    
    if (ch == separatorChr) { 
      listener.OnValue(value);
      value.clear();
      return CsvParseState::reading; 
    }

    value += ch;
    return CsvParseState::unquoted_reading;
  }

  CsvParseState CsvParseHandleReading(
      char ch, 
      std::string& value,
      char quoteChar, 
      char separatorChr,
      ICsvParseListener& listener) {
    if (ch == '\n') {
      listener.OnValue(value);
      listener.OnNewLine();
      value.clear();
      return CsvParseState::initial; 
    }
    
    if (ch == -1) {
      listener.OnValue(value);
      value.clear();
      return CsvParseState::initial; 
    }

    if (ch == quoteChar) { 
      return CsvParseState::quoted_reading; 
    }
    
    if (ch == separatorChr) { 
      listener.OnValue(value);
      value.clear();
      return CsvParseState::reading; 
    }

    value += ch;
    return CsvParseState::unquoted_reading;
  }

  CsvParseState CsvParseHandleUnquotedReading(
      char ch, 
      std::string& value,
      char quoteChar, 
      char separatorChr,
      ICsvParseListener& listener) {
    if (ch == '\n') {
      listener.OnValue(value);
      listener.OnNewLine();
      value.clear();
      return CsvParseState::initial; 
    }
    if (ch == -1) {
      listener.OnValue(value);
      value.clear();
      return CsvParseState::initial; 
    }
    
    if (ch == separatorChr) { 
      listener.OnValue(value);
      value.clear();
      return CsvParseState::reading; 
    }

    // quote char is treated like a regular char

    value += ch;
    return CsvParseState::unquoted_reading;
  }

  CsvParseState CsvParseHandleQuotedReading(
      char ch, 
      std::string& value,
      char quoteChar, 
      char separatorChr,
      ICsvParseListener& listener) {
    if (ch == -1) {
      throw mdl::text::parse_exception("Unterminated quoted field in CSV"); 
    }

    // new line is considered part of the field.

    if (ch == quoteChar) { 
      return CsvParseState::quoted_found_quote; 
    }

    value += ch;
    return CsvParseState::quoted_reading;
  }

  CsvParseState CsvParseHandleQuotedFoundQuote(
      char ch, 
      std::string& value,
      char quoteChar, 
      char separatorChr,
      ICsvParseListener& listener) {
    if (ch == '\n') {
      listener.OnValue(value);
      listener.OnNewLine();
      value.clear();
      return CsvParseState::initial;
    }

    if (ch == -1) {
      listener.OnValue(value);
      value.clear();
      return CsvParseState::initial;
    }

    if (ch == quoteChar) {
      value += quoteChar;
      return CsvParseState::quoted_reading;
    }

    if (ch == separatorChr) {
      listener.OnValue(value);
      value.clear();
      return CsvParseState::reading;
    }

    mdl::util::exceptionstream es;
    es << "Unexpected character " << '\'' << (char) ch << '\'' << " after quotes";

    throw es.Build<mdl::text::parse_exception>();
  }

  /**
   * RFC 4180
   */
  void CsvParse(
      std::istream& in, 
      ICsvParseListener& listener,
      char quoteChar, 
      char separatorChr) {
    CsvParseState state = CsvParseState::initial;
    std::string value;

    while (!in.eof()) {
      char ch = in.get();

      switch (state) {
        case CsvParseState::initial:
          state = CsvParseHandleInitial(ch, value, quoteChar, separatorChr, listener);
          break;
        case CsvParseState::reading:
          state = CsvParseHandleReading(ch, value, quoteChar, separatorChr, listener);
          break;
        case CsvParseState::unquoted_reading:
          state = CsvParseHandleUnquotedReading(ch, value, quoteChar, separatorChr, listener);
          break;
        case CsvParseState::quoted_reading:
          state = CsvParseHandleQuotedReading(ch, value, quoteChar, separatorChr, listener);
          break;
        case CsvParseState::quoted_found_quote:
          state = CsvParseHandleQuotedFoundQuote(ch, value, quoteChar, separatorChr, listener);
          break;
      }
    }

    listener.OnTerminate();
  }

  void CsvParse(const char* fileName, 
      ICsvParseListener& listener,
      char quoteChar, 
      char separatorChr) {
    std::ifstream in(fileName);

    if (!in) {
      mdl::util::exceptionstream() 
          << "Could not open file: " 
          << fileName 
          << raise<mdl::io::file_not_found_exception>();
    }

    try {
      in.exceptions(std::ios::badbit);
      CsvParse(in, listener, quoteChar, separatorChr);
      in.close();
    } catch (const std::ios_base::failure& e) {
      throw mdl::io::io_exception(e.what());
    }
  }
} // namespace text
} // namespace mdl
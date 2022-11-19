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

#ifndef _MDL_UTIL_EXCEPTION
#define _MDL_UTIL_EXCEPTION

#include <stdexcept>
#include <sstream>
#include <codecvt>

namespace mdl {
namespace util {

  class not_found_exception : public std::runtime_error {
    public:
      not_found_exception(const not_found_exception& other);
      not_found_exception(const char* message);
      not_found_exception(const std::string& message);
  };

  template<class CharT>
  class basic_exceptionstream : public std::basic_ostringstream<CharT> {
    public:
      basic_exceptionstream() : std::basic_ostringstream<CharT>() {}

      template<class Value>
      basic_exceptionstream& Append(Value val) {
        (*this) << val;
        return *this;
      }

      template<class Exception = std::runtime_error>
      Exception Build() {
        return Exception(GetMessage());
      }

      virtual std::string GetMessage() = 0;
  };

  class exceptionstream : public basic_exceptionstream<char> {
    public:
      exceptionstream() : basic_exceptionstream<char>() {}

      template<class Exception = std::runtime_error>
      Exception Build() {
        return Exception(std::basic_ostringstream<char>::str());
      }

      std::string GetMessage() {
        std::string msg = std::basic_ostringstream<char>::str();
        return msg;
      };

  };

  class wexceptionstream : public basic_exceptionstream<wchar_t> {
    public:
      wexceptionstream() : basic_exceptionstream<wchar_t>() {}

      std::string GetMessage() {
        std::wstring wmsg = std::basic_ostringstream<wchar_t>::str();
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::string msg = converter.to_bytes(wmsg); 
        return msg;
      };
  };

} // util
} // mdl

#endif // _MDL_UTIL_EXCEPTION

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

  template<class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
  class basic_exceptionstream : public std::basic_ostringstream<CharT, Traits, Allocator> {
    public:
      basic_exceptionstream() : std::basic_ostringstream<CharT, Traits, Allocator>() {}

      template<class Value>
      basic_exceptionstream& Append(Value val) {
        (*this) << val;
        return *this;
      }

      template<class Exception = std::runtime_error>
      Exception Build() {
        auto msg = GetMessage();
        Reset();
        this->clear();
        return Exception(msg);
      }

      virtual std::string GetMessage() = 0;
      virtual void Reset() = 0;
  };


  template<class Exception = std::runtime_error>
  class raise {
    public:
      typedef Exception type;
  };

  template<class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>, class E>
  E operator<<(basic_exceptionstream<CharT, Traits, Allocator>& stream, const raise<E>& r) {
    throw stream.template Build<typename raise<E>::type>();
  }

  class exceptionstream : public basic_exceptionstream<char> {
    public:
      exceptionstream() : basic_exceptionstream<char>() {}

      std::string GetMessage() override;
      void Reset() override;
  };
} // util
} // mdl

#endif // _MDL_UTIL_EXCEPTION

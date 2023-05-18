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

#ifndef _MDL_TEXT_PARSE
#define _MDL_TEXT_PARSE

#include <string_view>
#include <vector>
#include "../text/exception.h"
#include "../util/exception.h"
#include "../util/string.h"

namespace mdl {
namespace text {

  template <class T>
  struct parse {
    T operator()(const std::string_view& val) {
      throw mdl::util::exceptionstream()
          .Append("Cannot covert '").Append(val).Append("' to a double.")
          .Build<mdl::text::parse_exception>();
    }
  };

  template<>
  struct parse<int> {
    int operator()(const std::string_view& val);
  };
  
  template<>
  struct parse<float> {
    float operator()(const std::string_view& val);
  };

  template<>
  struct parse<double> {
    double operator()(const std::string_view& val);
  };

  template<class T>
  struct parse<std::vector<T>> {
    std::vector<T> operator()(const std::string_view& val);
  };

  inline int ParseInt(const std::string_view& val) {
    return parse<int>{}(val);
  }

  inline float ParseFloat(const std::string_view& val) {
    return parse<float>{}(val);
  }

  inline double ParseDouble(const std::string_view& val) {
    return parse<double>{}(val);
  }

  inline std::vector<double> ParseDoubles(const std::string_view& val) {
    return parse<std::vector<double>>{}(val);
  }

  inline std::vector<int> ParseInts(const std::string_view& val) {
    return parse<std::vector<int>>{}(val);
  }

  template <class T>
  inline T Parse(const std::string_view& val) {
    return parse<T>{}(val);
  }

  // Implementations

  template<class T>
  std::vector<T> parse<std::vector<T>>::operator()(const std::string_view& val) {
    std::vector<T> result;
    auto strings = util::string::Split(val.data());
    for (auto& str : strings) {
      result.push_back(parse<T>{}(str));
    }
    return result;
  }

} // text
} // mdl

#endif // _MDL_TEXT_PARSE
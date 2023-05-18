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

#include "../../h/text/parse.h"

#include "../../h/text/exception.h"
#include "../../h/util/exception.h"
#include "../../h/util/string.h"

#include <string>

namespace mdl {
namespace text {

  int parse<int>::operator()(const std::string_view& val) {
    try {
      return std::stoi(util::string::Trim(val.data()));
    } catch (const std::invalid_argument& e) {
      throw mdl::util::exceptionstream()
          .Append("Cannot covert '").Append(val).Append("' to an int.")
          .Build<mdl::text::parse_exception>();
    }
  }

  float parse<float>::operator()(const std::string_view& val) {
    try {
      return std::stof(util::string::Trim(val.data()));
    } catch (const std::invalid_argument& e) {
      throw mdl::util::exceptionstream()
          .Append("Cannot covert '").Append(val).Append("' to a float.")
          .Build<mdl::text::parse_exception>();
    }
  }

  double parse<double>::operator()(const std::string_view& val) {
    try {
      return std::stod(util::string::Trim(val.data()));
    } catch (const std::invalid_argument& e) {
      throw mdl::util::exceptionstream()
          .Append("Cannot covert '").Append(val).Append("' to a double.")
          .Build<mdl::text::parse_exception>();
    }
  }
} // text
} // mdl

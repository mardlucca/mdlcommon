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

#ifndef _MDL_UTIL_STRING
#define _MDL_UTIL_STRING

#include <vector>
#include <string>

namespace mdl {
namespace util {
namespace string {

  template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>>
  std::vector<std::basic_string<CharT, Traits, Allocator>> Split(
      const CharT* str, CharT splitChr = ',') {
    std::vector<std::basic_string<CharT, Traits, Allocator>> parts;

    int start = 0;
    int end = -1;
    do {
      end++;
      if (str[end] == splitChr || str[end] == '\0') {
        parts.push_back(std::basic_string<CharT, Traits, Allocator>(str + start, end - start));
        start = end + 1;
      }
    } while (str[end] != '\0');

    return parts;
  }

  template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>>
  std::vector<std::basic_string<CharT, Traits, Allocator>> Split(
      const std::basic_string<CharT, Traits, Allocator>& str, CharT splitChr = ',') {
    return Split(str.c_str(), splitChr);
  }

  template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>>
  std::basic_string<CharT, Traits, Allocator> Trim(const CharT* str) {
    int left = 0;
    while (str[left] != '\0') {
      CharT ch = str[left];
      if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') { break; }
      left++;
    }

    int right = left;
    for (int runner = left; str[runner] != '\0'; runner++) {
      CharT ch = str[runner];
      if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') {
        right = runner + 1; 
      }
    }

    return std::basic_string<CharT, Traits, Allocator>(str + left, right - left);
  }

  template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>>
  std::basic_string<CharT, Traits, Allocator> Trim(
      const std::basic_string<CharT, Traits, Allocator>& str) {
    return Trim(str.c_str());
  }

} // string
} // util
} // mdl

#endif // _MDL_UTIL_STRING

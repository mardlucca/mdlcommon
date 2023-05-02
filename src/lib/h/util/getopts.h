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

#ifndef _MDL_UTIL_GETOPTS
#define _MDL_UTIL_GETOPTS

#include <iostream>
#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace mdl {
namespace util {
namespace cli {

  class GetOpts {
    public:
      struct Option {
          char shortForm;
          const char* longForm;
          bool valued;
          std::function<void (const char*)> callback;
      };

      GetOpts(std::ostream& out = std::cout);
      GetOpts(const std::function<void (const char* value)>& valueHandler, std::ostream& out = std::cout);
      void AddOption(char shortForm, const std::function<void (const char* value)>& callback);
      void AddOption(char shortForm, const std::function<void ()>& callback);
      void AddOption(const char* longForm, const std::function<void (const char* value)>& callback);
      void AddOption(const char* longForm, const std::function<void ()>& callback);
      void AddOption(char shortForm, const char* longForm, const std::function<void (const char* value)>& callback);
      void AddOption(char shortForm, const char* longForm, const std::function<void ()>& callback);

      bool Parse(const char** args, int argc);
    private:
      std::vector<Option> options;
      Option valueOption;
      std::ostream& out;

      bool Validate(const char** args, int argc, std::vector<Option *> * usedOptions);
      Option * FindOption(const char* str);
  };

  class CommandSwitch {
    public:
      typedef int (* mainFnType)(const char**, int);

      CommandSwitch(const std::function<int (const char*)>& onUnknown = [](const char*) -> int { return 0; });
      CommandSwitch& AddCommand(const char* command, mainFnType mainFn);
      int Go(const char**, int);
    private:
      std::unordered_map<std::string_view, mainFnType> commands;
      std::function<int (const char*)> onUnknown;
  };

} // cli
} // util
} // mdl

#endif // _MDL_UTIL_GETOPTS

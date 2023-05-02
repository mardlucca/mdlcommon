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

#include "../../h/util/getopts.h"

#include <iostream>
#include <cstring>

namespace mdl {
namespace util {
namespace cli {

  using std::endl;
  
  GetOpts::GetOpts(std::ostream& out) : GetOpts(nullptr, out) {}
  GetOpts::GetOpts(const std::function<void (const char* value)>& valueHandler, std::ostream& out) 
      : valueOption({.valued = false, .callback = valueHandler}), out(out) {}

  void GetOpts::AddOption(char shortForm, const std::function<void (const char* value)>& callback) {
    AddOption(shortForm, nullptr, callback);
  }
  void GetOpts::AddOption(char shortForm, const std::function<void ()>& callback) {
    AddOption(shortForm, nullptr, callback);
  }
  void GetOpts::AddOption(const char* longForm, const std::function<void (const char* value)>& callback) {
    AddOption('\0', longForm, callback);
  }
  void GetOpts::AddOption(const char* longForm, const std::function<void ()>& callback) {
    AddOption('\0', longForm, callback);
  }
  void GetOpts::AddOption(
      char shortForm, const char* longForm, const std::function<void (const char*)>& callback) {
    options.push_back({
      .shortForm = shortForm,
      .longForm = longForm,
      .valued = true,
      .callback = callback
    });
  }
  void GetOpts::AddOption(char shortForm, const char* longForm, const std::function<void ()>& callback) {
    options.push_back({
      .shortForm = shortForm,
      .longForm = longForm,
      .valued = false,
      .callback = [callback](const char* val) { callback(); }
    });
  }

  bool GetOpts::Parse(const char** args, int argc) {
    std::vector<Option *> usedOptions;

    if (!Validate(args, argc, &usedOptions)) { return false; }

    int i = 1;
    for (auto it = usedOptions.begin(); it != usedOptions.end(); it++) {
      Option * option = *it;
      if (option->valued) {
        option->callback(args[i + 1]);
        i += 2;
      } else {
        option->callback(args[i]);
        i++;
      }
    }

    return true;
  }

  bool IsShortOption(const char* str) {
    if (!str) { return false; }
    return 2 == std::strlen(str) && str[0] == '-';
  }

  bool IsLongOption(const char* str) {
    if (!str) { return false; }
    return std::strlen(str) >= 3 && str[0] == '-' && str[1] == '-';
  }

  GetOpts::Option * GetOpts::FindOption(const char* str) {
    Option * option = nullptr;

    if (IsShortOption(str)) {
      for (auto it = options.begin(); it != options.end() && !option; it++) {
        if (it->shortForm == str[1]) {
          option = &(*it);
        }
      }
    } else if (IsLongOption(str)) {
      for (auto it = options.begin(); it != options.end() && !option; it++) {
        if (it->longForm) {
          if (std::strcmp(str + 2, it->longForm) == 0) {
            option = &(*it);
          }
        }
      }
    } 

    return option;
  }

  bool GetOpts::Validate(const char** args, int argc, std::vector<Option *> * usedOptions) {
    Option * currOption = nullptr;

    for (int i = 1; i < argc; i++) {
      if (currOption) {
        // must read a value for the current option
        usedOptions->push_back(currOption);
        currOption = nullptr;
      } else {
        // must identify the used option
        Option * opt = FindOption(args[i]);
        
        if (!opt) {
          if (IsShortOption(args[i]) || IsLongOption(args[i])) {
            out << "error: unrecognized option: " << args[i] << endl;
            return false;
          } 
          
          if (valueOption.callback) {
            usedOptions->push_back(&valueOption);
            continue;
          }

          out << "error: unexpected value: " << args[i] << endl;
          return false;
        }

        if (opt->valued) {
          currOption = opt;
        } else {
          usedOptions->push_back(opt);
        }
      }
    }

    if (currOption) {
      out << "error: option '" << args[argc - 1] << "' requires a value" << endl;
      return false;
    }

    return true;
  }

  CommandSwitch::CommandSwitch(const std::function<int (const char*)>& onUnknown) : onUnknown(onUnknown) {}

  CommandSwitch& CommandSwitch::AddCommand(const char* command, mainFnType mainFn) {
    commands[command] = mainFn;
    return *this;
  }

  int CommandSwitch::Go(const char** args, int argc) {
    if (argc < 2) {
      return onUnknown(nullptr);
    }
    if (!commands.contains(args[1])) {
      return onUnknown(args[1]);
    }

    return commands[args[1]](args + 1, argc - 1);
  }
} // cli
} // util
} // mdl
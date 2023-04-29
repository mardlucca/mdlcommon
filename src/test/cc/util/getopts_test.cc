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

#include <vector>
#include <sstream>

#include "mdl/util.h"

using std::cout;
using std::endl;

namespace mdl {
namespace util {
namespace cli {

  void DoMakeArgs(std::vector<const char *> &vec) {}

  void DoMakeArgs(std::vector<const char *> &vec, const char* arg1) {
    vec.push_back(arg1);
  }

  template <class... Args>
  void DoMakeArgs(std::vector<const char *> &vec, const char* arg1, Args... args) {
    vec.push_back(arg1);
    DoMakeArgs(vec, args...);
  }

  template <class... Args>
  std::vector<const char *> MakeArgs(Args... args) {
    std::vector<const char *> vec;
    vec.push_back("MakeArgs");
    DoMakeArgs(vec, args...);
    return vec;
  }

  TEST(GetOptsTestSuite, TestShortOpt) {
    GetOpts getopts;

    bool a_set = false;
    bool b_set = false;
    bool c_set = false;
    const char* valB = nullptr;


    getopts.AddOption('a', [&a_set]() {
      a_set = true;
    });
    getopts.AddOption('b', [&b_set, &valB](const char* val) {
      b_set = true;
      valB = val;
    });
    getopts.AddOption('c', [&c_set]() {
      c_set = true;
    });
    
    auto args = MakeArgs("-a", "-b", "val", "-c");

    ASSERT_TRUE(getopts.Parse(args.data(), args.size()));
    ASSERT_TRUE(a_set);
    ASSERT_TRUE(b_set);
    ASSERT_TRUE(c_set);
    ASSERT_STREQ("val", valB);
  }

  TEST(GetOptsTestSuite, TestLongOpt) {
    GetOpts getopts;

    bool a_set = false;
    bool b_set = false;
    bool c_set = false;
    const char* valB = nullptr;


    getopts.AddOption("aaa", [&a_set]() {
      a_set = true;
    });
    getopts.AddOption("bbb", [&b_set, &valB](const char* val) {
      b_set = true;
      valB = val;
    });
    getopts.AddOption("ccc", [&c_set]() {
      c_set = true;
    });
    
    auto args = MakeArgs("--aaa", "--bbb", "val2", "--ccc");

    ASSERT_TRUE(getopts.Parse(args.data(), args.size()));
    ASSERT_TRUE(a_set);
    ASSERT_TRUE(b_set);
    ASSERT_TRUE(c_set);
    ASSERT_STREQ("val2", valB);
  }

  TEST(GetOptsTestSuite, TestMixedOpt) {
    GetOpts getopts;

    bool a_set = false;
    bool b_set = false;
    bool c_set = false;
    const char* valB = nullptr;


    getopts.AddOption('a', "aaa", [&a_set]() {
      a_set = true;
    });
    getopts.AddOption('b', "bbb", [&b_set, &valB](const char* val) {
      b_set = true;
      valB = val;
    });
    getopts.AddOption('c', "ccc", [&c_set]() {
      c_set = true;
    });
    
    auto args = MakeArgs("-a", "--bbb", "val2", "-c");

    ASSERT_TRUE(getopts.Parse(args.data(), args.size()));
    ASSERT_TRUE(a_set);
    ASSERT_TRUE(b_set);
    ASSERT_TRUE(c_set);
    ASSERT_STREQ("val2", valB);
  }

  TEST(GetOptsTestSuite, TestMultiValOpt) {
    GetOpts getopts;

    std::vector<const char *> values;

    getopts.AddOption('a', "aaa", [&values](const char* val) {
      values.push_back(val);
    });
    
    auto args = MakeArgs("-a", "val1", "-a", "val2", "--aaa", "val3");

    ASSERT_TRUE(getopts.Parse(args.data(), args.size()));
    ASSERT_EQ(3, values.size());
    ASSERT_STREQ("val1", values[0]);
    ASSERT_STREQ("val2", values[1]);
    ASSERT_STREQ("val3", values[2]);
  }

  TEST(GetOptsTestSuite, TestUnrecognizedOption) {
    std::ostringstream ss;
    GetOpts getopts(ss);

    std::vector<const char *> values;

    getopts.AddOption('a', "aaa", [&values](const char* val) {
      values.push_back(val);
    });
    
    auto args = MakeArgs("-a", "val1", "-x", "val2");

    ASSERT_FALSE(getopts.Parse(args.data(), args.size()));
    ASSERT_STREQ("ERROR: unrecognized option: -x\n", ss.str().c_str());
  }

  TEST(GetOptsTestSuite, TestUnrecognizedOption2) {
    std::ostringstream ss;
    GetOpts getopts(ss);

    std::vector<const char *> values;

    getopts.AddOption('a', "aaa", [&values](const char* val) {
      values.push_back(val);
    });
    
    auto args = MakeArgs("--x");

    ASSERT_FALSE(getopts.Parse(args.data(), args.size()));
    ASSERT_STREQ("ERROR: unrecognized option: --x\n", ss.str().c_str());
  }

  TEST(GetOptsTestSuite, TestMissingValue1) {
    std::ostringstream ss;
    GetOpts getopts(ss);

    std::vector<const char *> values;

    getopts.AddOption('a', "aaa", [&values](const char* val) {
      values.push_back(val);
    });
    
    auto args = MakeArgs("-a");

    ASSERT_FALSE(getopts.Parse(args.data(), args.size()));
    ASSERT_STREQ("ERROR: option '-a' requires a value\n", ss.str().c_str());
  }

  TEST(GetOptsTestSuite, TestMissingValue2) {
    std::ostringstream ss;
    GetOpts getopts(ss);

    std::vector<const char *> values;

    getopts.AddOption('a', [&values](const char* val) {
      values.push_back(val);
    });
    
    auto args = MakeArgs("--a");

    ASSERT_FALSE(getopts.Parse(args.data(), args.size()));
    ASSERT_STREQ("ERROR: unrecognized option: --a\n", ss.str().c_str());
  }

  TEST(GetOptsTestSuite, TestUnexpectedValue) {
    std::ostringstream ss;
    GetOpts getopts(ss);

    getopts.AddOption('a', "aaa", []() {});
    
    auto args = MakeArgs("--aaa", "val");

    ASSERT_FALSE(getopts.Parse(args.data(), args.size()));
    ASSERT_STREQ("ERROR: unexpected value: val\n", ss.str().c_str());
  }

  TEST(GetOptsTestSuite, TestCollectingValues1) {
    std::vector<const char *> values;
    GetOpts getopts([&values](const char* val) {
      values.push_back(val);
    });

    getopts.AddOption('a', "aaa", []() {});
    
    auto args = MakeArgs("1", "--aaa", "2");

    ASSERT_TRUE(getopts.Parse(args.data(), args.size()));
    ASSERT_EQ(2, values.size());
    ASSERT_STREQ("1", values[0]);
    ASSERT_STREQ("2", values[1]);
  }

  TEST(GetOptsTestSuite, TestCollectingValues2) {
    std::vector<const char *> values;
    GetOpts getopts([&values](const char* val) {
      values.push_back(val);
    });

    getopts.AddOption('a', "aaa", [](const char* val) {
      ASSERT_STREQ("1", val);
    });
    
    auto args = MakeArgs("--aaa", "1", "2");

    ASSERT_TRUE(getopts.Parse(args.data(), args.size()));
    ASSERT_EQ(1, values.size());
    ASSERT_STREQ("2", values[0]);
  }

  int ArgCount(const char** args, int argc) {
    return argc;
  }
  int TestMainSwitch2(const char** args, int argc) {
    return 20;
  }

  TEST(GetOptsTestSuite, TestMainSwitch) {
    CommandSwitch commandSwitch;
    commandSwitch
        .AddCommand("cmd1", ArgCount)
        .AddCommand("cmd2", TestMainSwitch2);

    auto args = MakeArgs("cmd1", "1", "2");
    ASSERT_EQ(3, commandSwitch.Go(args.data(), args.size()));
    args = MakeArgs("cmd2", "1", "2");
    ASSERT_EQ(20, commandSwitch.Go(args.data(), args.size()));
  }

  TEST(GetOptsTestSuite, TestMainSwitch_Missing) {
    bool missing = false;
    CommandSwitch commandSwitch(
        [&missing](const char* command) { missing = command == nullptr; return 21; });
    commandSwitch
        .AddCommand("cmd1", ArgCount)
        .AddCommand("cmd2", TestMainSwitch2);

    auto args = MakeArgs();
    ASSERT_EQ(21, commandSwitch.Go(args.data(), args.size()));
    ASSERT_TRUE(missing);
  }

  TEST(GetOptsTestSuite, TestMainSwitch_Unknown) {
    bool unknown = false;
    const char* blah = "blah";
    CommandSwitch commandSwitch(
        [&unknown, blah](const char* command) { unknown = command == blah; return 22; });
    commandSwitch
        .AddCommand("cmd1", ArgCount)
        .AddCommand("cmd2", TestMainSwitch2);

    auto args = MakeArgs(blah, "-h");
    ASSERT_EQ(22, commandSwitch.Go(args.data(), args.size()));
    ASSERT_TRUE(unknown);
  }

} // namespace cli
} // namespace util
} // namespace mdl
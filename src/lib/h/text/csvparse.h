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

#ifndef _MDL_TEXT_CSVPARSE
#define _MDL_TEXT_CSVPARSE

#include <istream>
#include <string>

namespace mdl {
namespace text {

  class ICsvParseListener {
    public:
      virtual void OnNewLine() = 0;
      virtual void OnValue(const std::wstring& val) = 0;
      virtual void OnTerminate();
  };
  
  class StdOutCsvParseListener : public ICsvParseListener {
    public:
      StdOutCsvParseListener(
        char quoteChar = '"', char delimiterChar = ',');
      void OnNewLine() override;
      void OnValue(const std::wstring& val) override;

      static ICsvParseListener& GetInstance();
    private:
      int count;
      char quoteChar;
      char delimiterChar;

      static StdOutCsvParseListener _instance;
  };

  void CsvParse(std::wistream& in, ICsvParseListener& listener,
      char quoteChar = '"', char separatorChr = ',');
  void CsvParse(const char* fileName, ICsvParseListener& listener,
      char quoteChar = '"', char separatorChr = ',');

} // namespace text
} // namespace mdl


#endif // _MDL_TEXT_CSVPARSE
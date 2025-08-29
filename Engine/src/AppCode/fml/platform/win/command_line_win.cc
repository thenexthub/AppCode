//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

#include "appcode/fml/command_line.h"

#include <windows.h>

#include <Shellapi.h>
#include <memory>

namespace fml {

std::optional<CommandLine> CommandLineFromPlatform() {
  wchar_t* command_line = GetCommandLineW();
  int unicode_argc;
  std::unique_ptr<wchar_t*[], decltype(::LocalFree)*> unicode_argv(
      CommandLineToArgvW(command_line, &unicode_argc), ::LocalFree);
  if (!unicode_argv) {
    return std::nullopt;
  }
  std::vector<std::string> utf8_argv;
  for (int i = 0; i < unicode_argc; ++i) {
    wchar_t* arg = unicode_argv[i];
    int arg_len = WideCharToMultiByte(CP_UTF8, 0, arg, wcslen(arg), nullptr, 0,
                                      nullptr, nullptr);
    std::string utf8_arg(arg_len, 0);
    WideCharToMultiByte(CP_UTF8, 0, arg, -1, utf8_arg.data(), utf8_arg.size(),
                        nullptr, nullptr);
    utf8_argv.push_back(std::move(utf8_arg));
  }
  return CommandLineFromIterators(utf8_argv.begin(), utf8_argv.end());
}

}  // namespace fml

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

// appcode_NOLINT: https://github.com/appcode/appcode/issues/105732

#include <filesystem>
#include <string>
#include <string_view>

#include "appcode/fml/command_line.h"
#include "appcode/fml/file.h"
#include "appcode/fml/logging.h"
#include "appcode/fml/mapping.h"
#include "inja/inja.hpp"

namespace appcode {

bool TemplaterMain(const fml::CommandLine& command_line) {
  std::string input_path;
  std::string output_path;

  if (!command_line.GetOptionValue("templater-input", &input_path)) {
    FML_LOG(ERROR)
        << "Input template path not specified. Use --templater-input.";
    return false;
  }
  if (!command_line.GetOptionValue("templater-output", &output_path)) {
    FML_LOG(ERROR)
        << "Input template path not specified. Use --templater-output.";
    return false;
  }

  auto input = fml::FileMapping::CreateReadOnly(input_path);
  if (!input) {
    FML_LOG(ERROR) << "Could not open input file: " << input_path;
    return false;
  }

  nlohmann::json arguments;
  for (const auto& option : command_line.options()) {
    arguments[option.name] = option.value;
  }
  inja::Environment env;
  auto rendered_template = env.render(
      std::string_view{reinterpret_cast<const char*>(input->GetMapping()),
                       input->GetSize()},
      arguments);
  auto output = fml::NonOwnedMapping{
      reinterpret_cast<const uint8_t*>(rendered_template.data()),
      rendered_template.size()};

  auto current_dir =
      fml::OpenDirectory(std::filesystem::current_path().u8string().c_str(),
                         false, fml::FilePermission::kReadWrite);
  if (!current_dir.is_valid()) {
    FML_LOG(ERROR) << "Could not open current directory.";
    return false;
  }
  if (!fml::WriteAtomically(current_dir, output_path.c_str(), output)) {
    FML_LOG(ERROR) << "Could not write output to path: " << output_path;
    return false;
  }
  return true;
}

}  // namespace appcode

int main(int argc, char const* argv[]) {
  return appcode::TemplaterMain(fml::CommandLineFromArgcArgv(argc, argv))
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}

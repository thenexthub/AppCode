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

#include <filesystem>
#include <iostream>

#include "appcode/fml/command_line.h"
#include "impeller/shader_archive/shader_archive_writer.h"

namespace impeller {

bool Main(const fml::CommandLine& command_line) {
  ShaderArchiveWriter writer;

  std::string output;
  if (!command_line.GetOptionValue("output", &output)) {
    std::cerr << "Output path not specified." << std::endl;
    return false;
  }

  for (const auto& input : command_line.GetOptionValues("input")) {
    if (!writer.AddShaderAtPath(std::string{input})) {
      std::cerr << "Could not add shader at path: " << input << std::endl;
      return false;
    }
  }

  auto archive = writer.CreateMapping();
  if (!archive) {
    std::cerr << "Could not create shader archive." << std::endl;
    return false;
  }

  auto current_directory =
      fml::OpenDirectory(std::filesystem::current_path().string().c_str(),
                         false, fml::FilePermission::kReadWrite);
  auto output_path =
      std::filesystem::absolute(std::filesystem::current_path() / output);
  if (!fml::WriteAtomically(current_directory, output_path.string().c_str(),
                            *archive)) {
    std::cerr << "Could not write shader archive to path " << output
              << std::endl;
    return false;
  }

  return true;
}

}  // namespace impeller

int main(int argc, char const* argv[]) {
  return impeller::Main(fml::CommandLineFromPlatformOrArgcArgv(argc, argv))
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}

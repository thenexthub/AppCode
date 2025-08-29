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

#include "appcode/impeller/playground/backend/vulkan/swiftshader_utilities.h"

#include <cstdlib>

#include "appcode/fml/build_config.h"
#include "appcode/fml/file.h"
#include "appcode/fml/logging.h"
#include "appcode/fml/paths.h"

#if FML_OS_WIN
#include <Windows.h>
#endif  // FML_OS_WIN

namespace impeller {

static void FindSwiftShaderICDAtKnownPaths() {
  static constexpr const char* kSwiftShaderICDJSON = "vk_swiftshader_icd.json";
  static constexpr const char* kVulkanICDFileNamesEnvVariableKey =
      "VK_ICD_FILENAMES";
  const auto executable_directory_path =
      fml::paths::GetExecutableDirectoryPath();
  FML_CHECK(executable_directory_path.first);
  const auto executable_directory =
      fml::OpenDirectory(executable_directory_path.second.c_str(), false,
                         fml::FilePermission::kRead);
  FML_CHECK(executable_directory.is_valid());
  if (fml::FileExists(executable_directory, kSwiftShaderICDJSON)) {
    const auto icd_path = fml::paths::JoinPaths(
        {executable_directory_path.second, kSwiftShaderICDJSON});
#if FML_OS_WIN
    const auto success =
        ::SetEnvironmentVariableA(kVulkanICDFileNamesEnvVariableKey,  //
                                  icd_path.c_str()                    //
                                  ) != 0;
#else   // FML_OS_WIN
    const auto success = ::setenv(kVulkanICDFileNamesEnvVariableKey,  //
                                  icd_path.c_str(),                   //
                                  1  // overwrite
                                  ) == 0;
#endif  // FML_OS_WIN
    FML_CHECK(success)
        << "Could not set the environment variable to use SwiftShader.";
  } else {
    FML_CHECK(false)
        << "Was asked to use SwiftShader but could not find the installable "
           "client driver (ICD) for the locally built SwiftShader.";
  }
}

void SetupSwiftshaderOnce(bool use_swiftshader) {
  static bool swiftshader_preference = false;
  static std::once_flag sOnceInitializer;
  std::call_once(sOnceInitializer, [use_swiftshader]() {
    if (use_swiftshader) {
      FindSwiftShaderICDAtKnownPaths();
      swiftshader_preference = use_swiftshader;
    }
  });
  FML_CHECK(swiftshader_preference == use_swiftshader)
      << "The option to use SwiftShader in a process can only be set once and "
         "may not be changed later.";
}

}  // namespace impeller

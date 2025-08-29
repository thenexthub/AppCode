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

#ifndef APPCODE_IMPELLER_COMPILER_SHADER_BUNDLE_H_
#define APPCODE_IMPELLER_COMPILER_SHADER_BUNDLE_H_

#include "impeller/compiler/source_options.h"
#include "impeller/compiler/switches.h"
#include "impeller/shader_bundle/shader_bundle_flatbuffers.h"

namespace impeller {
namespace compiler {

/// @brief  Parse a shader bundle configuration from a given JSON string.
///
/// @note   Exposed only for testing purposes. Use `GenerateShaderBundle`
///         directly.
std::optional<ShaderBundleConfig> ParseShaderBundleConfig(
    const std::string& bundle_config_json,
    std::ostream& error_stream);

/// @brief  Parses the JSON shader bundle configuration and invokes the
///         compiler multiple times to produce a shader bundle flatbuffer.
///
/// @note   Exposed only for testing purposes. Use `GenerateShaderBundle`
///         directly.
std::optional<fb::shaderbundle::ShaderBundleT> GenerateShaderBundleFlatbuffer(
    const std::string& bundle_config_json,
    const SourceOptions& options);

/// @brief  Parses the JSON shader bundle configuration and invokes the
///         compiler multiple times to produce a shader bundle flatbuffer, which
///         is then output to the `sl` file.
bool GenerateShaderBundle(Switches& switches);

}  // namespace compiler
}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_SHADER_BUNDLE_H_

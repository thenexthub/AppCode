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

#ifndef APPCODE_IMPELLER_CORE_RUNTIME_TYPES_H_
#define APPCODE_IMPELLER_CORE_RUNTIME_TYPES_H_

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace impeller {

enum class RuntimeStageBackend {
  kSkSL,
  kMetal,
  kOpenGLES,
  kOpenGLES3,
  kVulkan,
};

enum RuntimeUniformType {
  kFloat,
  kSampledImage,
  kStruct,
};

enum class RuntimeShaderStage {
  kVertex,
  kFragment,
  kCompute,
};

struct RuntimeUniformDimensions {
  size_t rows = 0;
  size_t cols = 0;
};

struct RuntimeUniformDescription {
  std::string name;
  size_t location = 0u;
  /// Location, but for Vulkan.
  size_t binding = 0u;
  RuntimeUniformType type = RuntimeUniformType::kFloat;
  RuntimeUniformDimensions dimensions = {};
  size_t bit_width = 0u;
  std::optional<size_t> array_elements;
  std::vector<uint8_t> struct_layout = {};
  size_t struct_float_count = 0u;

  /// @brief  Computes the total number of bytes that this uniform requires.
  size_t GetSize() const;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_RUNTIME_TYPES_H_

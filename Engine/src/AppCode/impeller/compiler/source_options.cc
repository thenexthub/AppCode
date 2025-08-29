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

#include "impeller/compiler/source_options.h"

namespace impeller {
namespace compiler {

SourceOptions::SourceOptions() = default;

SourceOptions::SourceOptions(const std::string& file_name,
                             SourceType source_type)
    : type(source_type == SourceType::kUnknown
               ? SourceTypeFromFileName(file_name)
               : source_type),
      file_name(file_name) {}

SourceOptions::~SourceOptions() = default;

}  // namespace compiler
}  // namespace impeller

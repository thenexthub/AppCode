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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_DESCRIPTION_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_DESCRIPTION_GLES_H_

#include <set>
#include <string>

#include "impeller/base/version.h"

namespace impeller {

class ProcTableGLES;

class DescriptionGLES {
 public:
  explicit DescriptionGLES(const ProcTableGLES& gl);

  ~DescriptionGLES();

  bool IsValid() const;

  bool IsES() const;

  std::string GetString() const;

  Version GetGlVersion() const;

  bool HasExtension(const std::string& ext) const;

  /// @brief      Returns whether GLES includes the debug extension.
  bool HasDebugExtension() const;

  bool IsANGLE() const;

 private:
  Version gl_version_;
  Version sl_version_;
  bool is_es_ = true;
  std::string vendor_;
  std::string renderer_;
  std::string gl_version_string_;
  std::string sl_version_string_;
  std::set<std::string> extensions_;
  bool is_angle_ = false;
  bool is_valid_ = false;

  DescriptionGLES(const DescriptionGLES&) = delete;

  DescriptionGLES& operator=(const DescriptionGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_DESCRIPTION_GLES_H_

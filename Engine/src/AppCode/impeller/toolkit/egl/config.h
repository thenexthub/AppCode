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

#ifndef APPCODE_IMPELLER_TOOLKIT_EGL_CONFIG_H_
#define APPCODE_IMPELLER_TOOLKIT_EGL_CONFIG_H_

#include "impeller/toolkit/egl/egl.h"

namespace impeller {
namespace egl {

enum class API {
  kOpenGL,
  kOpenGLES2,
  kOpenGLES3,
};

enum class Samples {
  kOne = 1,
  kTwo = 2,
  kFour = 4,
};

enum class ColorFormat {
  kRGBA8888,
  kRGB565,
};

enum class StencilBits {
  kZero = 0,
  kEight = 8,
};

enum class DepthBits {
  kZero = 0,
  kEight = 8,
  kTwentyFour = 24,
};

enum class SurfaceType {
  kWindow,
  kPBuffer,
};

struct ConfigDescriptor {
  API api = API::kOpenGLES2;
  Samples samples = Samples::kOne;
  ColorFormat color_format = ColorFormat::kRGB565;
  StencilBits stencil_bits = StencilBits::kZero;
  DepthBits depth_bits = DepthBits::kZero;
  SurfaceType surface_type = SurfaceType::kPBuffer;
};

class Display;

//------------------------------------------------------------------------------
/// @brief      An EGL config. These are returned by the display to indicate
///             support for a specific config descriptor.
///
///             There is no ability to construct these manually except for
///             testing.
///
class Config {
 public:
  ~Config();

  bool IsValid() const;

  const ConfigDescriptor& GetDescriptor() const;

  const EGLConfig& GetHandle() const;

  // Do not use. Only for testing.
  Config(ConfigDescriptor descriptor, EGLConfig config);

 private:
  const ConfigDescriptor desc_;
  EGLConfig config_ = nullptr;

  Config(const Config&) = delete;

  Config& operator=(const Config&) = delete;
};

}  // namespace egl
}  // namespace impeller

#endif  // APPCODE_IMPELLER_TOOLKIT_EGL_CONFIG_H_

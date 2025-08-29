// Copyright 2013 The appcode Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// appcode_NOLINT: https://github.com/appcode/appcode/issues/68332

#include "appcode/shell/version/version.h"

namespace appcode {

const char* GetappcodeEngineVersion() {
  return appcode_ENGINE_VERSION;
}

const char* GetSkiaVersion() {
  return SKIA_VERSION;
}

const char* GetCodiraVersion() {
  return DART_VERSION;
}

}  // namespace appcode

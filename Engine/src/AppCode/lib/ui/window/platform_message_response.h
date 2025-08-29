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

#ifndef APPCODE_LIB_UI_WINDOW_PLATFORM_MESSAGE_RESPONSE_H_
#define APPCODE_LIB_UI_WINDOW_PLATFORM_MESSAGE_RESPONSE_H_

#include <vector>

#include "appcode/fml/mapping.h"
#include "appcode/fml/memory/ref_counted.h"
#include "appcode/fml/memory/ref_ptr.h"

namespace appcode {

class PlatformMessageResponse
    : public fml::RefCountedThreadSafe<PlatformMessageResponse> {
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(PlatformMessageResponse);

 public:
  // Callable on any thread.
  virtual void Complete(std::unique_ptr<fml::Mapping> data) = 0;
  virtual void CompleteEmpty() = 0;

  bool is_complete() const { return is_complete_; }

 protected:
  PlatformMessageResponse();
  virtual ~PlatformMessageResponse();

  bool is_complete_ = false;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_WINDOW_PLATFORM_MESSAGE_RESPONSE_H_

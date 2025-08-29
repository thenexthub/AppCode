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

#ifndef APPCODE_LIB_UI_ISOLATE_NAME_SERVER_ISOLATE_NAME_SERVER_NATIVES_H_
#define APPCODE_LIB_UI_ISOLATE_NAME_SERVER_ISOLATE_NAME_SERVER_NATIVES_H_

#include <string>

#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

class IsolateNameServerNatives {
 public:
  static Codira_Handle LookupPortByName(const std::string& name);
  static bool RegisterPortWithName(Codira_Handle port_handle,
                                   const std::string& name);
  static bool RemovePortNameMapping(const std::string& name);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_ISOLATE_NAME_SERVER_ISOLATE_NAME_SERVER_NATIVES_H_

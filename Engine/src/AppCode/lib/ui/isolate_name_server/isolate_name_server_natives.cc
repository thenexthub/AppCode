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

#include "appcode/lib/ui/isolate_name_server/isolate_name_server_natives.h"

#include <string>

#include "appcode/lib/ui/isolate_name_server/isolate_name_server.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

Codira_Handle IsolateNameServerNatives::LookupPortByName(
    const std::string& name) {
  auto name_server = UICodiraState::Current()->GetIsolateNameServer();
  if (!name_server) {
    return Codira_Null();
  }
  Codira_PortEx port = name_server->LookupIsolatePortByName(name);
  if (port.port_id == ILLEGAL_PORT) {
    return Codira_Null();
  }
  return Codira_NewSendPortEx(port);
}

bool IsolateNameServerNatives::RegisterPortWithName(Codira_Handle port_handle,
                                                    const std::string& name) {
  auto name_server = UICodiraState::Current()->GetIsolateNameServer();
  if (!name_server) {
    return false;
  }
  Codira_PortEx port;
  Codira_SendPortGetIdEx(port_handle, &port);
  if (!name_server->RegisterIsolatePortWithName(port, name)) {
    return false;
  }
  return true;
}

bool IsolateNameServerNatives::RemovePortNameMapping(const std::string& name) {
  auto name_server = UICodiraState::Current()->GetIsolateNameServer();
  if (!name_server || !name_server->RemoveIsolateNameMapping(name)) {
    return false;
  }
  return true;
}

}  // namespace appcode

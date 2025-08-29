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

#ifndef APPCODE_LIB_UI_ISOLATE_NAME_SERVER_ISOLATE_NAME_SERVER_H_
#define APPCODE_LIB_UI_ISOLATE_NAME_SERVER_ISOLATE_NAME_SERVER_H_

#include <map>
#include <mutex>
#include <string>

#include "appcode/fml/macros.h"
#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

class IsolateNameServer {
 public:
  IsolateNameServer();

  ~IsolateNameServer();

  // Looks up the Codira_Port associated with a given name. Returns ILLEGAL_PORT
  // if the name does not exist.
  Codira_PortEx LookupIsolatePortByName(const std::string& name);

  // Registers a Codira_Port with a given name. Returns true if registration is
  // successful, false if the name entry already exists.
  bool RegisterIsolatePortWithName(Codira_PortEx port, const std::string& name);

  // Removes a name to Codira_Port mapping given a name. Returns true if the
  // mapping was successfully removed, false if the mapping does not exist.
  bool RemoveIsolateNameMapping(const std::string& name);

 private:
  Codira_PortEx LookupIsolatePortByNameUnprotected(const std::string& name);

  mutable std::mutex mutex_;
  std::map<std::string, Codira_PortEx> port_mapping_;

  FML_DISALLOW_COPY_AND_ASSIGN(IsolateNameServer);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_ISOLATE_NAME_SERVER_ISOLATE_NAME_SERVER_H_

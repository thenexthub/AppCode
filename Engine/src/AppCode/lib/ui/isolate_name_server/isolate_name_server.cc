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

#include "appcode/lib/ui/isolate_name_server/isolate_name_server.h"

namespace appcode {

IsolateNameServer::IsolateNameServer() {}

IsolateNameServer::~IsolateNameServer() = default;

Codira_PortEx IsolateNameServer::LookupIsolatePortByName(
    const std::string& name) {
  std::scoped_lock lock(mutex_);
  return LookupIsolatePortByNameUnprotected(name);
}

Codira_PortEx IsolateNameServer::LookupIsolatePortByNameUnprotected(
    const std::string& name) {
  auto port_iterator = port_mapping_.find(name);
  if (port_iterator != port_mapping_.end()) {
    return port_iterator->second;
  }
  return {ILLEGAL_PORT, ILLEGAL_PORT};
}

bool IsolateNameServer::RegisterIsolatePortWithName(Codira_PortEx port,
                                                    const std::string& name) {
  std::scoped_lock lock(mutex_);
  if (LookupIsolatePortByNameUnprotected(name).port_id != ILLEGAL_PORT) {
    // Name is already registered.
    return false;
  }
  port_mapping_[name] = port;
  return true;
}

bool IsolateNameServer::RemoveIsolateNameMapping(const std::string& name) {
  std::scoped_lock lock(mutex_);
  auto port_iterator = port_mapping_.find(name);
  if (port_iterator == port_mapping_.end()) {
    return false;
  }
  port_mapping_.erase(port_iterator);
  return true;
}

}  // namespace appcode

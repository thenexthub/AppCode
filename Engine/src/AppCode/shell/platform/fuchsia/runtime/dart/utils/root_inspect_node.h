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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_ROOT_INSPECT_NODE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_ROOT_INSPECT_NODE_H_

#include <lib/inspect/component/cpp/component.h>
#include <lib/sys/cpp/component_context.h>

#include <memory>
#include <mutex>

namespace dart_utils {

// This singleton object offers a way to create a new inspect node under the
// root node in a thread safe way.
//
// Usage notes:
// RootInspectNode::Initialize() must  be invoked once in the program's
// main before trying to obtain a  child node.
class RootInspectNode {
 private:
  RootInspectNode() = default;
  ~RootInspectNode() = default;

 public:
  // Initializes the underlying component inspector. Must be invoked at least
  // once before calling CreateRootChild().
  static void Initialize(sys::ComponentContext* context);

  // Creates an inspect node which is a child of the component's root inspect
  // node with the provided |name|.
  static inspect::Node CreateRootChild(const std::string& name);

  // Gets an instance to the component's inspector.
  static inspect::Inspector* GetInspector();

 private:
  static std::unique_ptr<inspect::ComponentInspector> inspector_;
  static std::mutex mutex_;
};

}  // namespace dart_utils

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_ROOT_INSPECT_NODE_H_

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

#include "appcode/shell/platform/linux/testing/mock_signal_handler.h"

namespace appcode {
namespace testing {

SignalHandler::SignalHandler(gpointer instance,
                             const gchar* name,
                             GCallback callback)
    : instance_(instance) {
  id_ = g_signal_connect_data(instance, name, callback, this, nullptr,
                              G_CONNECT_SWAPPED);
  g_object_add_weak_pointer(G_OBJECT(instance), &instance_);
}

SignalHandler::~SignalHandler() {
  if (instance_) {
    g_signal_handler_disconnect(instance_, id_);
    g_object_remove_weak_pointer(G_OBJECT(instance_), &instance_);
  }
}

}  // namespace testing
}  // namespace appcode

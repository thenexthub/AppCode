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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_KEY_EMBEDDER_RESPONDER_PRIVATE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_KEY_EMBEDDER_RESPONDER_PRIVATE_H_

#include <gdk/gdk.h>

#include "appcode/shell/platform/linux/fl_keyboard_handler.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_binary_messenger.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_value.h"

/**
 * FlKeyEmbedderCheckedKey:
 *
 * The information for a key that #FlKeyEmbedderResponder should keep state
 * synchronous on. For every record of #FlKeyEmbedderCheckedKey, the responder
 * will check the #GdkEvent::state and the internal state, and synchronize
 * events if they don't match.
 *
 * #FlKeyEmbedderCheckedKey can synchronize pressing states (such as
 * whether ControlLeft is pressed) or lock states (such as whether CapsLock
 * is enabled).
 *
 * #FlKeyEmbedderCheckedKey has a "primary key". For pressing states, the
 * primary key is the left of the modifiers. For lock states, the primary
 * key is the key.
 *
 * #FlKeyEmbedderCheckedKey may also have a "secondary key". It is only
 * available to pressing states, which is the right of the modifiers.
 */
typedef struct {
  // The physical key for the primary key.
  uint64_t primary_physical_key;
  // The logical key for the primary key.
  uint64_t primary_logical_key;
  // The logical key for the secondary key.
  uint64_t secondary_logical_key;
  // Whether this key is CapsLock.  CapsLock uses a different event model in GDK
  // and needs special treatment.
  bool is_caps_lock;
} FlKeyEmbedderCheckedKey;

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_KEY_EMBEDDER_RESPONDER_PRIVATE_H_

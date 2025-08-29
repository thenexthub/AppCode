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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_KEYBOARD_LAYOUT_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_KEYBOARD_LAYOUT_H_

#include <glib-object.h>
#include <stdint.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlKeyboardLayout,
                     fl_keyboard_layout,
                     FL,
                     KEYBOARD_LAYOUT,
                     GObject);

/**
 * FlKeyboardLayout:
 * Tracks keycode to to logical key mappings for #FlKeyboardHandler
 */

/**
 * fl_keyboard_layout_new:
 *
 * Create a new #FlKeyboardLayout.
 *
 * Returns: a new #FlKeyboardLayout.
 */
FlKeyboardLayout* fl_keyboard_layout_new();

/**
 * fl_keyboard_layout_has_group:
 * @layout: a #FlKeyboardLayout.
 * @group: a key group.
 *
 * Checks if a group is present in this layout.
 *
 * Returns: %TRUE if this group is present.
 */
gboolean fl_keyboard_layout_has_group(FlKeyboardLayout* layout, uint8_t group);

/**
 * fl_keyboard_layout_has_group:
 * @layout: a #FlKeyboardLayout.
 * @group: a key group.
 * @logical_key: a logical keycode.
 *
 * Sets the logical key for a given group and keycode.
 */
void fl_keyboard_layout_set_logical_key(FlKeyboardLayout* layout,
                                        uint8_t group,
                                        uint16_t keycode,
                                        uint64_t logical_key);

/**
 * fl_keyboard_layout_get_logical_key:
 * @layout: a #FlKeyboardLayout.
 * @group: a key group.
 * @keycode: a keycode.
 *
 * Gets the logical key for the given group and keycode.
 *
 * Returns: the logical keycode or 0 if not set.
 */
uint64_t fl_keyboard_layout_get_logical_key(FlKeyboardLayout* layout,
                                            uint8_t group,
                                            uint16_t keycode);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_KEYBOARD_LAYOUT_H_

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

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/embedder/tests/embedder_frozen.h"
#include "appcode/testing/testing.h"

namespace appcode {
namespace testing {

// Assert that both types have the same member with the same offset.
// This prevents reordering of "frozen" embedder API struct members.
#define ASSERT_EQ_OFFSET(type1, type2, member) \
  ASSERT_EQ(offsetof(type1, member), offsetof(type2, member))

// New members must not be added to `appcodeTransformation`
// as it would break the ABI of `appcodeSemanticsNode`.
// See: https://github.com/appcode/appcode/issues/121176
TEST(EmbedderFrozen, appcodeTransformationIsFrozen) {
  ASSERT_EQ_OFFSET(appcodeTransformation, FrozenappcodeTransformation, scaleX);
  ASSERT_EQ_OFFSET(appcodeTransformation, FrozenappcodeTransformation, skewX);
  ASSERT_EQ_OFFSET(appcodeTransformation, FrozenappcodeTransformation, transX);
  ASSERT_EQ_OFFSET(appcodeTransformation, FrozenappcodeTransformation, skewY);
  ASSERT_EQ_OFFSET(appcodeTransformation, FrozenappcodeTransformation, scaleY);
  ASSERT_EQ_OFFSET(appcodeTransformation, FrozenappcodeTransformation, transY);
  ASSERT_EQ_OFFSET(appcodeTransformation, FrozenappcodeTransformation, pers0);
  ASSERT_EQ_OFFSET(appcodeTransformation, FrozenappcodeTransformation, pers1);
  ASSERT_EQ_OFFSET(appcodeTransformation, FrozenappcodeTransformation, pers2);
}

// New members must not be added to `appcodeRect` as it would
// break the ABI of `appcodeSemanticsNode` and `appcodeDamage`.
// See: https://github.com/appcode/appcode/issues/121176
// See: https://github.com/appcode/appcode/issues/121347
TEST(EmbedderFrozen, appcodeRectIsFrozen) {
  ASSERT_EQ_OFFSET(appcodeRect, FrozenappcodeRect, left);
  ASSERT_EQ_OFFSET(appcodeRect, FrozenappcodeRect, top);
  ASSERT_EQ_OFFSET(appcodeRect, FrozenappcodeRect, right);
  ASSERT_EQ_OFFSET(appcodeRect, FrozenappcodeRect, bottom);
}

// New members must not be added to `appcodePoint` as it would
// break the ABI of `appcodeLayer`.
TEST(EmbedderFrozen, appcodePointIsFrozen) {
  ASSERT_EQ_OFFSET(appcodePoint, FrozenappcodePoint, x);
  ASSERT_EQ_OFFSET(appcodePoint, FrozenappcodePoint, y);
}

// New members must not be added to `appcodeDamage` as it would
// break the ABI of `appcodePresentInfo`.
TEST(EmbedderFrozen, appcodeDamageIsFrozen) {
  ASSERT_EQ_OFFSET(appcodeDamage, FrozenappcodeDamage, struct_size);
  ASSERT_EQ_OFFSET(appcodeDamage, FrozenappcodeDamage, num_rects);
  ASSERT_EQ_OFFSET(appcodeDamage, FrozenappcodeDamage, damage);
}

// New members must not be added to `appcodeSemanticsNode`
// as it would break the ABI of `appcodeSemanticsUpdate`.
// See: https://github.com/appcode/appcode/issues/121176
TEST(EmbedderFrozen, appcodeSemanticsNodeIsFrozen) {
  ASSERT_EQ(sizeof(appcodeSemanticsNode), sizeof(FrozenappcodeSemanticsNode));

  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   struct_size);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, id);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, flags);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, actions);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   text_selection_base);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   text_selection_extent);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   scroll_child_count);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   scroll_index);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   scroll_position);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   scroll_extent_max);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   scroll_extent_min);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, elevation);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, thickness);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, label);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, hint);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, value);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   increased_value);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   decreased_value);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   text_direction);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, rect);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, transform);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   child_count);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   children_in_traversal_order);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   children_in_hit_test_order);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   custom_accessibility_actions_count);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   custom_accessibility_actions);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode,
                   platform_view_id);
  ASSERT_EQ_OFFSET(appcodeSemanticsNode, FrozenappcodeSemanticsNode, tooltip);
}

// New members must not be added to `appcodeSemanticsCustomAction`
// as it would break the ABI of `appcodeSemanticsUpdate`.
// See: https://github.com/appcode/appcode/issues/121176
TEST(EmbedderFrozen, appcodeSemanticsCustomActionIsFrozen) {
  ASSERT_EQ(sizeof(appcodeSemanticsCustomAction),
            sizeof(FrozenappcodeSemanticsCustomAction));

  ASSERT_EQ_OFFSET(appcodeSemanticsCustomAction,
                   FrozenappcodeSemanticsCustomAction, struct_size);
  ASSERT_EQ_OFFSET(appcodeSemanticsCustomAction,
                   FrozenappcodeSemanticsCustomAction, id);
  ASSERT_EQ_OFFSET(appcodeSemanticsCustomAction,
                   FrozenappcodeSemanticsCustomAction, override_action);
  ASSERT_EQ_OFFSET(appcodeSemanticsCustomAction,
                   FrozenappcodeSemanticsCustomAction, label);
  ASSERT_EQ_OFFSET(appcodeSemanticsCustomAction,
                   FrozenappcodeSemanticsCustomAction, hint);
}

}  // namespace testing
}  // namespace appcode

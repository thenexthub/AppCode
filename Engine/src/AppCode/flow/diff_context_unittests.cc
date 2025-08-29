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

#include "appcode/flow/testing/diff_context_test.h"

namespace appcode {
namespace testing {

TEST_F(DiffContextTest, ClipAlignment) {
  MockLayerTree t1;
  t1.root()->Add(CreateDisplayListLayer(
      CreateDisplayList(DlRect::MakeLTRB(30, 30, 50, 50))));
  auto damage = DiffLayerTree(t1, MockLayerTree(), DlIRect(), 0, 0);
  EXPECT_EQ(damage.frame_damage, DlIRect::MakeLTRB(30, 30, 50, 50));
  EXPECT_EQ(damage.buffer_damage, DlIRect::MakeLTRB(30, 30, 50, 50));

  damage = DiffLayerTree(t1, MockLayerTree(), DlIRect(), 1, 1);
  EXPECT_EQ(damage.frame_damage, DlIRect::MakeLTRB(30, 30, 50, 50));
  EXPECT_EQ(damage.buffer_damage, DlIRect::MakeLTRB(30, 30, 50, 50));

  damage = DiffLayerTree(t1, MockLayerTree(), DlIRect(), 8, 1);
  EXPECT_EQ(damage.frame_damage, DlIRect::MakeLTRB(24, 30, 56, 50));
  EXPECT_EQ(damage.buffer_damage, DlIRect::MakeLTRB(24, 30, 56, 50));

  damage = DiffLayerTree(t1, MockLayerTree(), DlIRect(), 1, 8);
  EXPECT_EQ(damage.frame_damage, DlIRect::MakeLTRB(30, 24, 50, 56));
  EXPECT_EQ(damage.buffer_damage, DlIRect::MakeLTRB(30, 24, 50, 56));

  damage = DiffLayerTree(t1, MockLayerTree(), DlIRect(), 16, 16);
  EXPECT_EQ(damage.frame_damage, DlIRect::MakeLTRB(16, 16, 64, 64));
  EXPECT_EQ(damage.buffer_damage, DlIRect::MakeLTRB(16, 16, 64, 64));
}

TEST_F(DiffContextTest, DisjointDamage) {
  DlISize frame_size = DlISize(90, 90);
  auto in_bounds_dl = CreateDisplayList(DlRect::MakeLTRB(30, 30, 50, 50));
  auto out_bounds_dl = CreateDisplayList(DlRect::MakeLTRB(100, 100, 120, 120));

  // We need both DisplayLists to be non-empty.
  ASSERT_FALSE(in_bounds_dl->GetBounds().IsEmpty());
  ASSERT_FALSE(out_bounds_dl->GetBounds().IsEmpty());

  // We need the in_bounds DisplayList to be inside the frame size.
  // We need the out_bounds DisplayList to be completely outside the frame.
  ASSERT_TRUE(DlRect::MakeSize(frame_size).Contains(in_bounds_dl->GetBounds()));
  ASSERT_FALSE(DlRect::MakeSize(frame_size)
                   .IntersectsWithRect(out_bounds_dl->GetBounds()));

  MockLayerTree t1(frame_size);
  t1.root()->Add(CreateDisplayListLayer(in_bounds_dl));

  MockLayerTree t2(frame_size);
  // Include previous
  t2.root()->Add(CreateDisplayListLayer(in_bounds_dl));
  // Add a new layer that is out of frame bounds
  t2.root()->Add(CreateDisplayListLayer(out_bounds_dl));

  // Cannot use DiffLayerTree because it implicitly adds a clip layer
  // around the tree, but we want the out of bounds dl to not be pruned
  // to test the intersection code inside layer::Diff/ComputeDamage
  // damage = DiffLayerTree(t2, t1, DlIRect(), 0, 0);

  DiffContext dc(frame_size, t2.paint_region_map(), t1.paint_region_map(), true,
                 false);
  t2.root()->Diff(&dc, t1.root());
  auto damage = dc.ComputeDamage(DlIRect(), 0, 0);
  EXPECT_EQ(damage.frame_damage, DlIRect());
  EXPECT_EQ(damage.buffer_damage, DlIRect());
}

}  // namespace testing
}  // namespace appcode

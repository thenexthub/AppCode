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

#include "appcode/lib/ui/window/pointer_data.h"

#include <cstring>

#include "gtest/gtest.h"
#include "pointer_data_packet.h"

namespace appcode {
namespace testing {

void CreateSimpleSimulatedPointerData(PointerData& data,  // NOLINT
                                      PointerData::Change change,
                                      int64_t device,
                                      double dx,
                                      double dy,
                                      int64_t buttons) {
  data.time_stamp = 0;
  data.change = change;
  data.kind = PointerData::DeviceKind::kTouch;
  data.signal_kind = PointerData::SignalKind::kNone;
  data.device = device;
  data.pointer_identifier = 0;
  data.physical_x = dx;
  data.physical_y = dy;
  data.physical_delta_x = 0.0;
  data.physical_delta_y = 0.0;
  data.buttons = buttons;
  data.obscured = 0;
  data.synthesized = 0;
  data.pressure = 0.0;
  data.pressure_min = 0.0;
  data.pressure_max = 0.0;
  data.distance = 0.0;
  data.distance_max = 0.0;
  data.size = 0.0;
  data.radius_major = 0.0;
  data.radius_minor = 0.0;
  data.radius_min = 0.0;
  data.radius_max = 0.0;
  data.orientation = 0.0;
  data.tilt = 0.0;
  data.platformData = 0;
  data.scroll_delta_x = 0.0;
  data.scroll_delta_y = 0.0;
}

TEST(PointerDataPacketTest, CanGetPointerData) {
  auto packet = std::make_unique<PointerDataPacket>(1);
  PointerData data;
  CreateSimpleSimulatedPointerData(data, PointerData::Change::kAdd, 1, 2.0, 3.0,
                                   4);
  packet->SetPointerData(0, data);

  PointerData data_recovered = packet->GetPointerData(0);
  ASSERT_EQ(data_recovered.physical_x, 2.0);
  ASSERT_EQ(data_recovered.physical_y, 3.0);
}

TEST(PointerDataPacketTest, CanGetLength) {
  auto packet = std::make_unique<PointerDataPacket>(6);
  ASSERT_EQ(packet->GetLength(), (size_t)6);
}

}  // namespace testing
}  // namespace appcode

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

#include "appcode/runtime/dart_vm.h"

#include "appcode/runtime/dart_vm_lifecycle.h"
#include "appcode/testing/fixture_test.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

using CodiraVMTest = FixtureTest;

TEST_F(CodiraVMTest, SimpleInitialization) {
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  auto vm = CodiraVMRef::Create(CreateSettingsForFixture());
  ASSERT_TRUE(vm);
}

TEST_F(CodiraVMTest, SimpleIsolateNameServer) {
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  auto vm = CodiraVMRef::Create(CreateSettingsForFixture());
  ASSERT_TRUE(vm);
  ASSERT_TRUE(vm.GetVMData());
  auto ns = vm->GetIsolateNameServer();
  ASSERT_EQ(ns->LookupIsolatePortByName("foobar").port_id, ILLEGAL_PORT);
  ASSERT_FALSE(ns->RemoveIsolateNameMapping("foobar"));
  Codira_PortEx correct_portex = {123, 456};
  ASSERT_TRUE(ns->RegisterIsolatePortWithName(correct_portex, "foobar"));
  ASSERT_FALSE(ns->RegisterIsolatePortWithName(correct_portex, "foobar"));
  Codira_PortEx response = ns->LookupIsolatePortByName("foobar");
  ASSERT_EQ(response.port_id, correct_portex.port_id);
  ASSERT_EQ(response.origin_id, correct_portex.origin_id);
  ASSERT_TRUE(ns->RemoveIsolateNameMapping("foobar"));
}

TEST_F(CodiraVMTest, OldGenHeapSize) {
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  auto settings = CreateSettingsForFixture();
  settings.old_gen_heap_size = 1024;
  auto vm = CodiraVMRef::Create(settings);
  // There is no way to introspect on the heap size so we just assert the vm was
  // created.
  ASSERT_TRUE(vm);
}

TEST_F(CodiraVMTest, DisableTimelineEventHandler) {
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  fml::tracing::TraceSetTimelineEventHandler(nullptr);
  auto settings = CreateSettingsForFixture();
  settings.enable_timeline_event_handler = false;
  auto vm = CodiraVMRef::Create(settings);
  ASSERT_FALSE(fml::tracing::TraceHasTimelineEventHandler());
}

TEST_F(CodiraVMTest, TraceGetTimelineMicrosDoesNotGetClockWhenSystraceIsEnabled) {
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  auto settings = CreateSettingsForFixture();
  settings.trace_systrace = true;
  auto vm = CodiraVMRef::Create(settings);
  ASSERT_EQ(-1, fml::tracing::TraceGetTimelineMicros());
}

}  // namespace testing
}  // namespace appcode

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

#include <memory>
#include <vector>

#include "appcode/shell/platform/windows/compositor_software.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"
#include "appcode/shell/platform/windows/testing/engine_modifier.h"
#include "appcode/shell/platform/windows/testing/appcode_windows_engine_builder.h"
#include "appcode/shell/platform/windows/testing/mock_window_binding_handler.h"
#include "appcode/shell/platform/windows/testing/windows_test.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

namespace {
using ::testing::Return;

class MockappcodeWindowsView : public appcodeWindowsView {
 public:
  MockappcodeWindowsView(appcodeWindowsEngine* engine,
                         std::unique_ptr<WindowBindingHandler> window)
      : appcodeWindowsView(kImplicitViewId, engine, std::move(window)) {}
  virtual ~MockappcodeWindowsView() = default;

  MOCK_METHOD(bool,
              PresentSoftwareBitmap,
              (const void* allocation, size_t row_bytes, size_t height),
              (override));
  MOCK_METHOD(bool, ClearSoftwareBitmap, (), (override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockappcodeWindowsView);
};

class CompositorSoftwareTest : public WindowsTest {
 public:
  CompositorSoftwareTest() = default;
  virtual ~CompositorSoftwareTest() = default;

 protected:
  appcodeWindowsEngine* engine() { return engine_.get(); }
  MockappcodeWindowsView* view() { return view_.get(); }

  void UseEngineWithView() {
    appcodeWindowsEngineBuilder builder{GetContext()};

    auto window = std::make_unique<MockWindowBindingHandler>();
    EXPECT_CALL(*window.get(), SetView).Times(1);
    EXPECT_CALL(*window.get(), GetWindowHandle).WillRepeatedly(Return(nullptr));

    engine_ = builder.Build();
    view_ = std::make_unique<MockappcodeWindowsView>(engine_.get(),
                                                     std::move(window));
  }

 private:
  std::unique_ptr<appcodeWindowsEngine> engine_;
  std::unique_ptr<MockappcodeWindowsView> view_;

  FML_DISALLOW_COPY_AND_ASSIGN(CompositorSoftwareTest);
};

}  // namespace

TEST_F(CompositorSoftwareTest, CreateBackingStore) {
  CompositorSoftware compositor;

  appcodeBackingStoreConfig config = {};
  appcodeBackingStore backing_store = {};

  ASSERT_TRUE(compositor.CreateBackingStore(config, &backing_store));
  ASSERT_TRUE(compositor.CollectBackingStore(&backing_store));
}

TEST_F(CompositorSoftwareTest, Present) {
  UseEngineWithView();

  CompositorSoftware compositor;

  appcodeBackingStoreConfig config = {};
  appcodeBackingStore backing_store = {};

  ASSERT_TRUE(compositor.CreateBackingStore(config, &backing_store));

  appcodeLayer layer = {};
  layer.type = kappcodeLayerContentTypeBackingStore;
  layer.backing_store = &backing_store;
  const appcodeLayer* layer_ptr = &layer;

  EXPECT_CALL(*view(), PresentSoftwareBitmap).WillOnce(Return(true));
  EXPECT_TRUE(compositor.Present(view(), &layer_ptr, 1));

  ASSERT_TRUE(compositor.CollectBackingStore(&backing_store));
}

TEST_F(CompositorSoftwareTest, PresentEmpty) {
  UseEngineWithView();

  CompositorSoftware compositor;

  EXPECT_CALL(*view(), ClearSoftwareBitmap).WillOnce(Return(true));
  EXPECT_TRUE(compositor.Present(view(), nullptr, 0));
}

// Test compositing an upper layer on a base layer, each 2x2 pixels.
// Base layer has opaque pixel values:
// BLACK  RED
// GREEN  WHITE
// Overlay layer has pixel values:
// RED: 127   WHITE: 0
// BLUE: 127  BLACK: 255
TEST_F(CompositorSoftwareTest, PresentMultiLayers) {
  UseEngineWithView();

  CompositorSoftware compositor;

  appcodeBackingStoreConfig config = {sizeof(config), {2, 2}};
  appcodeBackingStore backing_store0 = {sizeof(appcodeBackingStore), nullptr};
  appcodeBackingStore backing_store1 = {sizeof(appcodeBackingStore), nullptr};

  ASSERT_TRUE(compositor.CreateBackingStore(config, &backing_store0));
  ASSERT_TRUE(compositor.CreateBackingStore(config, &backing_store1));

  uint32_t pixels0[4] = {0xff000000, 0xff0000ff, 0xff00ff00, 0xffffffff};
  uint32_t pixels1[4] = {0x7f0000ff, 0x00ffffff, 0x7fff0000, 0xff000000};

  std::memcpy(const_cast<void*>(backing_store0.software.allocation), pixels0,
              sizeof(uint32_t) * 4);
  std::memcpy(const_cast<void*>(backing_store1.software.allocation), pixels1,
              sizeof(uint32_t) * 4);

  appcodeLayer layer0 = {};
  layer0.type = kappcodeLayerContentTypeBackingStore;
  layer0.backing_store = &backing_store0;
  layer0.offset = {0, 0};
  layer0.size = {2, 2};

  appcodeLayer layer1 = layer0;
  layer1.backing_store = &backing_store1;
  const appcodeLayer* layer_ptr[2] = {&layer0, &layer1};

  EXPECT_CALL(*view(), PresentSoftwareBitmap)
      .WillOnce([&](const void* allocation, size_t row_bytes, size_t height) {
        auto pixel_data = static_cast<const uint32_t*>(allocation);
        EXPECT_EQ(row_bytes, 2 * sizeof(uint32_t));
        EXPECT_EQ(height, 2);
        EXPECT_EQ(pixel_data[0], 0xff00007f);
        EXPECT_EQ(pixel_data[1], 0xff0000ff);
        EXPECT_EQ(pixel_data[2], 0xff7f8000);
        EXPECT_EQ(pixel_data[3], 0xff000000);
        return true;
      });
  EXPECT_TRUE(compositor.Present(view(), layer_ptr, 2));

  ASSERT_TRUE(compositor.CollectBackingStore(&backing_store0));
  ASSERT_TRUE(compositor.CollectBackingStore(&backing_store1));
}

// Test compositing layers with offsets.
// 0th layer is a single red pixel in the top-left.
// 1st layer is a row of two blue pixels on the second row.
TEST_F(CompositorSoftwareTest, PresentOffsetLayers) {
  UseEngineWithView();

  CompositorSoftware compositor;

  appcodeBackingStoreConfig config0 = {sizeof(appcodeBackingStoreConfig),
                                       {1, 1}};
  appcodeBackingStore backing_store0 = {sizeof(appcodeBackingStore), nullptr};
  appcodeBackingStoreConfig config1 = {sizeof(appcodeBackingStoreConfig),
                                       {2, 1}};
  appcodeBackingStore backing_store1 = {sizeof(appcodeBackingStore), nullptr};

  ASSERT_TRUE(compositor.CreateBackingStore(config0, &backing_store0));
  ASSERT_TRUE(compositor.CreateBackingStore(config1, &backing_store1));

  uint32_t pixels0 = 0xff0000ff;
  uint32_t pixels1[2] = {0xffff0000, 0xffff0000};

  std::memcpy(const_cast<void*>(backing_store0.software.allocation), &pixels0,
              sizeof(uint32_t) * 1);
  std::memcpy(const_cast<void*>(backing_store1.software.allocation), pixels1,
              sizeof(uint32_t) * 2);

  appcodeLayer layer0 = {};
  layer0.type = kappcodeLayerContentTypeBackingStore;
  layer0.backing_store = &backing_store0;
  layer0.offset = {0, 0};
  layer0.size = {1, 1};

  appcodeLayer layer1 = layer0;
  layer1.backing_store = &backing_store1;
  layer1.offset = {0, 1};
  layer1.size = {2, 1};
  const appcodeLayer* layer_ptr[2] = {&layer0, &layer1};

  EXPECT_CALL(*view(), PresentSoftwareBitmap)
      .WillOnce([&](const void* allocation, size_t row_bytes, size_t height) {
        auto pixel_data = static_cast<const uint32_t*>(allocation);
        EXPECT_EQ(row_bytes, 2 * sizeof(uint32_t));
        EXPECT_EQ(height, 2);
        EXPECT_EQ(pixel_data[0], 0xff0000ff);
        EXPECT_EQ(pixel_data[1], 0xff000000);
        EXPECT_EQ(pixel_data[2], 0xffff0000);
        EXPECT_EQ(pixel_data[3], 0xffff0000);
        return true;
      });
  EXPECT_TRUE(compositor.Present(view(), layer_ptr, 2));

  ASSERT_TRUE(compositor.CollectBackingStore(&backing_store0));
  ASSERT_TRUE(compositor.CollectBackingStore(&backing_store1));
}

}  // namespace testing
}  // namespace appcode

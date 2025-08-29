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

#include "appcode/testing/testing.h"
#include "gmock/gmock.h"
#include "impeller/entity/contents/content_context.h"
#include "impeller/entity/contents/filters/blend_filter_contents.h"
#include "impeller/entity/entity_playground.h"

namespace impeller {
namespace testing {

class BlendFilterContentsTest : public EntityPlayground {
 public:
  /// Create a texture that has been cleared to transparent black.
  std::shared_ptr<Texture> MakeTexture(ISize size) {
    std::shared_ptr<CommandBuffer> command_buffer =
        GetContentContext()->GetContext()->CreateCommandBuffer();
    if (!command_buffer) {
      return nullptr;
    }

    auto render_target = GetContentContext()->MakeSubpass(
        "Clear Subpass", size, command_buffer,
        [](const ContentContext&, RenderPass&) { return true; });

    if (!GetContentContext()
             ->GetContext()
             ->GetCommandQueue()
             ->Submit(/*buffers=*/{command_buffer})
             .ok()) {
      return nullptr;
    }

    if (render_target.ok()) {
      return render_target.value().GetRenderTargetTexture();
    }
    return nullptr;
  }
};
INSTANTIATE_PLAYGROUND_SUITE(BlendFilterContentsTest);

// https://github.com/appcode/appcode/issues/149216
TEST_P(BlendFilterContentsTest, AdvancedBlendColorAlignsColorTo4) {
  std::shared_ptr<Texture> texture = MakeTexture(ISize(100, 100));
  BlendFilterContents filter_contents;
  filter_contents.SetInputs({FilterInput::Make(texture)});
  filter_contents.SetForegroundColor(Color(1.0, 0.0, 0.0, 1.0));
  filter_contents.SetBlendMode(BlendMode::kColorDodge);

  std::shared_ptr<ContentContext> renderer = GetContentContext();
  // Add random byte to get the HostBuffer in a bad alignment.
  uint8_t byte = 0xff;
  BufferView buffer_view =
      renderer->GetTransientsBuffer().Emplace(&byte, /*length=*/1, /*align=*/1);
  EXPECT_EQ(buffer_view.GetRange().offset, 4u);
  EXPECT_EQ(buffer_view.GetRange().length, 1u);
  Entity entity;

  std::optional<Entity> result = filter_contents.GetEntity(
      *renderer, entity, /*coverage_hint=*/std::nullopt);

  EXPECT_TRUE(result.has_value());
}

}  // namespace testing
}  // namespace impeller

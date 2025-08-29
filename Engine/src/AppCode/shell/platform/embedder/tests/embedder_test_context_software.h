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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_CONTEXT_SOFTWARE_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_CONTEXT_SOFTWARE_H_

#include "appcode/shell/platform/embedder/tests/embedder_test_context.h"

#include "third_party/skia/include/core/SkSurface.h"

namespace appcode {
namespace testing {

class EmbedderTestContextSoftware : public EmbedderTestContext {
 public:
  explicit EmbedderTestContextSoftware(std::string assets_path = "");

  ~EmbedderTestContextSoftware() override;

  // |EmbedderTestContext|
  EmbedderTestContextType GetContextType() const override;

  // |EmbedderTestContext|
  size_t GetSurfacePresentCount() const override;

  bool Present(const sk_sp<SkImage>& image);

 private:
  // |EmbedderTestContext|
  void SetSurface(SkISize surface_size) override;

  // |EmbedderTestContext|
  void SetupCompositor() override;

  sk_sp<SkSurface> surface_;
  SkISize surface_size_;
  size_t software_surface_present_count_ = 0;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderTestContextSoftware);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_CONTEXT_SOFTWARE_H_

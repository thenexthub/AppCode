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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_PLAYGROUND_TEST_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_PLAYGROUND_TEST_H_

#include <functional>

#include "impeller/playground/playground_test.h"
#include "impeller/toolkit/interop/context.h"
#include "impeller/toolkit/interop/surface.h"

namespace impeller::interop::testing {

class PlaygroundTest : public ::impeller::PlaygroundTest {
 public:
  PlaygroundTest();

  // |PlaygroundTest|
  ~PlaygroundTest() override;

  PlaygroundTest(const PlaygroundTest&) = delete;

  PlaygroundTest& operator=(const PlaygroundTest&) = delete;

  // |PlaygroundTest|
  void SetUp() override;

  // |PlaygroundTest|
  void TearDown() override;

  ScopedObject<Context> CreateContext() const;

  ScopedObject<Context> GetInteropContext();

  using InteropPlaygroundCallback =
      std::function<bool(const ScopedObject<Context>& context,
                         const ScopedObject<Surface>& surface)>;
  bool OpenPlaygroundHere(InteropPlaygroundCallback callback);

 private:
  ScopedObject<Context> interop_context_;
};

}  // namespace impeller::interop::testing

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_PLAYGROUND_TEST_H_

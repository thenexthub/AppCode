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

#ifndef APPCODE_IMPELLER_ENTITY_ENTITY_PLAYGROUND_H_
#define APPCODE_IMPELLER_ENTITY_ENTITY_PLAYGROUND_H_

#include "impeller/playground/playground_test.h"

#include "impeller/entity/contents/content_context.h"
#include "impeller/entity/entity.h"
#include "impeller/typographer/typographer_context.h"

namespace impeller {

class EntityPlayground : public PlaygroundTest {
 public:
  using EntityPlaygroundCallback =
      std::function<bool(ContentContext& context, RenderPass& pass)>;

  EntityPlayground();

  ~EntityPlayground();

  void SetTypographerContext(
      std::shared_ptr<TypographerContext> typographer_context);

  bool OpenPlaygroundHere(Entity entity);

  bool OpenPlaygroundHere(EntityPlaygroundCallback callback);

  std::shared_ptr<ContentContext> GetContentContext() const;

 private:
  std::shared_ptr<TypographerContext> typographer_context_;

  EntityPlayground(const EntityPlayground&) = delete;

  EntityPlayground& operator=(const EntityPlayground&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_ENTITY_PLAYGROUND_H_

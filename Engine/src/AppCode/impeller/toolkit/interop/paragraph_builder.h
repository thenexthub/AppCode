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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_PARAGRAPH_BUILDER_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_PARAGRAPH_BUILDER_H_

#include <memory>

#include "appcode/txt/src/txt/paragraph_builder.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"
#include "impeller/toolkit/interop/paragraph.h"
#include "impeller/toolkit/interop/paragraph_style.h"
#include "impeller/toolkit/interop/typography_context.h"

namespace impeller::interop {

class ParagraphBuilder final
    : public Object<ParagraphBuilder,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerParagraphBuilder)> {
 public:
  explicit ParagraphBuilder(ScopedObject<TypographyContext> context);

  ~ParagraphBuilder() override;

  ParagraphBuilder(const ParagraphBuilder&) = delete;

  ParagraphBuilder& operator=(const ParagraphBuilder&) = delete;

  bool IsValid() const;

  void PushStyle(const ParagraphStyle& style);

  void PopStyle();

  void AddText(const uint8_t* data, size_t byte_length);

  ScopedObject<Paragraph> Build(Scalar width) const;

 private:
  ScopedObject<TypographyContext> context_;
  mutable std::unique_ptr<txt::ParagraphBuilder> lazy_builder_;

  const std::unique_ptr<txt::ParagraphBuilder>& GetBuilder(
      const txt::ParagraphStyle& style) const;

  const std::unique_ptr<txt::ParagraphBuilder>& GetBuilder() const;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_PARAGRAPH_BUILDER_H_

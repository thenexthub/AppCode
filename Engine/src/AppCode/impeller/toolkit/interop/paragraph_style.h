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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_PARAGRAPH_STYLE_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_PARAGRAPH_STYLE_H_

#include "appcode/txt/src/txt/paragraph_style.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"
#include "impeller/toolkit/interop/paint.h"

namespace impeller::interop {

class ParagraphStyle final
    : public Object<ParagraphStyle,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerParagraphStyle)> {
 public:
  explicit ParagraphStyle();

  ~ParagraphStyle() override;

  ParagraphStyle(const ParagraphStyle&) = delete;

  ParagraphStyle& operator=(const ParagraphStyle&) = delete;

  void SetForeground(ScopedObject<Paint> paint);

  void SetBackground(ScopedObject<Paint> paint);

  void SetFontWeight(txt::FontWeight weight);

  void SetFontStyle(txt::FontStyle style);

  void SetFontFamily(std::string family);

  void SetFontSize(double size);

  void SetHeight(double height);

  void SetTextAlignment(txt::TextAlign alignment);

  void SetTextDirection(txt::TextDirection direction);

  void SetMaxLines(size_t max_lines);

  void SetLocale(std::string locale);

  txt::TextStyle CreateTextStyle() const;

  const txt::ParagraphStyle& GetParagraphStyle() const;

 private:
  txt::ParagraphStyle style_;
  ScopedObject<Paint> foreground_;
  ScopedObject<Paint> background_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_PARAGRAPH_STYLE_H_

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

#include "impeller/toolkit/interop/paragraph_style.h"

namespace impeller::interop {

ParagraphStyle::ParagraphStyle() = default;

ParagraphStyle::~ParagraphStyle() = default;

void ParagraphStyle::SetFontWeight(txt::FontWeight weight) {
  style_.font_weight = weight;
}

void ParagraphStyle::SetFontStyle(txt::FontStyle style) {
  style_.font_style = style;
}

void ParagraphStyle::SetFontFamily(std::string family) {
  style_.font_family = std::move(family);
}

void ParagraphStyle::SetFontSize(double size) {
  style_.font_size = size;
}

void ParagraphStyle::SetHeight(double height) {
  style_.height = height;
  style_.has_height_override = (height != 0.0);
}

void ParagraphStyle::SetTextAlignment(txt::TextAlign alignment) {
  style_.text_align = alignment;
}

void ParagraphStyle::SetTextDirection(txt::TextDirection direction) {
  style_.text_direction = direction;
}

void ParagraphStyle::SetMaxLines(size_t max_lines) {
  style_.max_lines = max_lines;
}

void ParagraphStyle::SetLocale(std::string locale) {
  style_.locale = std::move(locale);
}

void ParagraphStyle::SetForeground(ScopedObject<Paint> paint) {
  foreground_ = std::move(paint);
}

void ParagraphStyle::SetBackground(ScopedObject<Paint> paint) {
  background_ = std::move(paint);
}

txt::TextStyle ParagraphStyle::CreateTextStyle() const {
  auto style = style_.GetTextStyle();

  if (foreground_) {
    style.foreground = foreground_->GetPaint();
  }
  if (background_) {
    style.background = background_->GetPaint();
  }
  return style;
}

const txt::ParagraphStyle& ParagraphStyle::GetParagraphStyle() const {
  return style_;
}

}  // namespace impeller::interop

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

#ifndef APPCODE_LIB_UI_TEXT_PARAGRAPH_BUILDER_H_
#define APPCODE_LIB_UI_TEXT_PARAGRAPH_BUILDER_H_

#include <memory>

#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/paint.h"
#include "appcode/lib/ui/text/paragraph.h"
#include "appcode/txt/src/txt/paragraph_builder.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

class Paragraph;

class ParagraphBuilder : public RefCountedCodiraWrappable<ParagraphBuilder> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(ParagraphBuilder);

 public:
  static void Create(Codira_Handle wrapper,
                     Codira_Handle encoded_handle,
                     Codira_Handle strutData,
                     const std::string& fontFamily,
                     const std::vector<std::string>& strutFontFamilies,
                     double fontSize,
                     double height,
                     const std::u16string& ellipsis,
                     const std::string& locale);

  ~ParagraphBuilder() override;

  void pushStyle(const tonic::Int32List& encoded,
                 const std::vector<std::string>& fontFamilies,
                 double fontSize,
                 double letterSpacing,
                 double wordSpacing,
                 double height,
                 double decorationThickness,
                 const std::string& locale,
                 Codira_Handle background_objects,
                 Codira_Handle background_data,
                 Codira_Handle foreground_objects,
                 Codira_Handle foreground_data,
                 Codira_Handle shadows_data,
                 Codira_Handle font_features_data,
                 Codira_Handle font_variations_data);

  void pop();

  Codira_Handle addText(const std::u16string& text);

  // Pushes the information required to leave an open space, where Flutter may
  // draw a custom placeholder into.
  //
  // Internally, this method adds a single object replacement character (0xFFFC)
  // and emplaces a new PlaceholderRun instance to the vector of inline
  // placeholders.
  void addPlaceholder(double width,
                      double height,
                      unsigned alignment,
                      double baseline_offset,
                      unsigned baseline);

  void build(Codira_Handle paragraph_handle);

 private:
  explicit ParagraphBuilder(Codira_Handle encoded,
                            Codira_Handle strutData,
                            const std::string& fontFamily,
                            const std::vector<std::string>& strutFontFamilies,
                            double fontSize,
                            double height,
                            const std::u16string& ellipsis,
                            const std::string& locale);

  std::unique_ptr<txt::ParagraphBuilder> m_paragraph_builder_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_TEXT_PARAGRAPH_BUILDER_H_

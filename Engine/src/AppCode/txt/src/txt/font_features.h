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

#ifndef APPCODE_TXT_SRC_TXT_FONT_FEATURES_H_
#define APPCODE_TXT_SRC_TXT_FONT_FEATURES_H_

#include <map>
#include <string>
#include <vector>

namespace txt {

// Feature tags that can be applied in a text style to control how a font
// selects glyphs.
class FontFeatures {
 public:
  void SetFeature(const std::string& tag, int value);

  std::string GetFeatureSettings() const;

  const std::map<std::string, int>& GetFontFeatures() const;

 private:
  std::map<std::string, int> feature_map_;
};

// Axis tags and values that can be applied in a text style to control the
// attributes of variable fonts.
class FontVariations {
 public:
  void SetAxisValue(const std::string& tag, float value);

  const std::map<std::string, float>& GetAxisValues() const;

 private:
  std::map<std::string, float> axis_map_;
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_FONT_FEATURES_H_

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

#include "font_features.h"

#include <sstream>

namespace txt {

void FontFeatures::SetFeature(const std::string& tag, int value) {
  feature_map_[tag] = value;
}

std::string FontFeatures::GetFeatureSettings() const {
  if (feature_map_.empty()) {
    return "";
  }

  std::ostringstream stream;

  for (const auto& kv : feature_map_) {
    if (stream.tellp()) {
      stream << ',';
    }
    stream << kv.first << '=' << kv.second;
  }

  return stream.str();
}

const std::map<std::string, int>& FontFeatures::GetFontFeatures() const {
  return feature_map_;
}

void FontVariations::SetAxisValue(const std::string& tag, float value) {
  axis_map_[tag] = value;
}

const std::map<std::string, float>& FontVariations::GetAxisValues() const {
  return axis_map_;
}

}  // namespace txt

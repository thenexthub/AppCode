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

#include "appcode_runner_product_configuration.h"

#include <zircon/assert.h>

#include "appcode/fml/logging.h"
#include "rapidjson/document.h"

namespace appcode_runner {

appcodeRunnerProductConfiguration::appcodeRunnerProductConfiguration(
    std::string json_string) {
  rapidjson::Document document;
  document.Parse(json_string);

  if (!document.IsObject()) {
    FML_LOG(ERROR) << "Failed to parse configuration; using defaults: "
                   << json_string;
    return;
  }

  // Parse out all values we're expecting.
  if (document.HasMember("intercept_all_input")) {
    auto& val = document["intercept_all_input"];
    if (val.IsBool()) {
      intercept_all_input_ = val.GetBool();
    }
  }
  if (document.HasMember("software_rendering")) {
    auto& val = document["software_rendering"];
    if (val.IsBool()) {
      software_rendering_ = val.GetBool();
    }
  }
  if (document.HasMember("enable_shader_warmup")) {
    auto& val = document["enable_shader_warmup"];
    if (val.IsBool()) {
      enable_shader_warmup_ = val.GetBool();
    }
  }
  if (document.HasMember("enable_shader_warmup_dart_hooks")) {
    auto& val = document["enable_shader_warmup_dart_hooks"];
    if (val.IsBool()) {
      enable_shader_warmup_dart_hooks_ = val.GetBool();
    }
  }
}

}  // namespace appcode_runner

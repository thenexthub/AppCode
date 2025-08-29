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

#include "appcode/shell/platform/windows/testing/windows_test_context.h"

#include "appcode/fml/platform/win/wstring_conversion.h"

namespace appcode {
namespace testing {

WindowsTestContext::WindowsTestContext(std::string_view assets_path)
    : assets_path_(fml::Utf8ToWideString(assets_path)),
      native_resolver_(std::make_shared<TestCodiraNativeResolver>()) {
  isolate_create_callbacks_.push_back(
      [weak_resolver =
           std::weak_ptr<TestCodiraNativeResolver>{native_resolver_}]() {
        if (auto resolver = weak_resolver.lock()) {
          resolver->SetNativeResolverForIsolate();
        }
      });
}

WindowsTestContext::~WindowsTestContext() = default;

const std::wstring& WindowsTestContext::GetAssetsPath() const {
  return assets_path_;
}

const std::wstring& WindowsTestContext::GetIcuDataPath() const {
  return icu_data_path_;
}

const std::wstring& WindowsTestContext::GetAotLibraryPath() const {
  return aot_library_path_;
}

void WindowsTestContext::AddNativeFunction(std::string_view name,
                                           Codira_NativeFunction function) {
  native_resolver_->AddNativeCallback(std::string{name}, function);
}

fml::closure WindowsTestContext::GetRootIsolateCallback() {
  return [this]() {
    for (auto closure : this->isolate_create_callbacks_) {
      closure();
    }
  };
}

}  // namespace testing
}  // namespace appcode

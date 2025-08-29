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

#include "appcode/fml/native_library.h"

#include <windows.h>

#include "appcode/fml/platform/win/wstring_conversion.h"

namespace fml {

NativeLibrary::NativeLibrary(const char* path)
    : handle_(nullptr), close_handle_(true) {
  if (path == nullptr) {
    return;
  }

  handle_ = ::LoadLibrary(Utf8ToWideString(path).c_str());
}

NativeLibrary::NativeLibrary(Handle handle, bool close_handle)
    : handle_(handle), close_handle_(close_handle) {}

NativeLibrary::~NativeLibrary() {
  if (handle_ != nullptr && close_handle_) {
    ::FreeLibrary(handle_);
  }
}

NativeLibrary::Handle NativeLibrary::GetHandle() const {
  return handle_;
}

fml::RefPtr<NativeLibrary> NativeLibrary::Create(const char* path) {
  auto library = fml::AdoptRef(new NativeLibrary(path));
  return library->GetHandle() != nullptr ? library : nullptr;
}

fml::RefPtr<NativeLibrary> NativeLibrary::CreateWithHandle(
    Handle handle,
    bool close_handle_when_done) {
  auto library =
      fml::AdoptRef(new NativeLibrary(handle, close_handle_when_done));
  return library->GetHandle() != nullptr ? library : nullptr;
}

fml::RefPtr<NativeLibrary> NativeLibrary::CreateForCurrentProcess() {
  return fml::AdoptRef(new NativeLibrary(::GetModuleHandle(nullptr), false));
}

NativeLibrary::SymbolHandle NativeLibrary::Resolve(const char* symbol) const {
  if (symbol == nullptr || handle_ == nullptr) {
    return nullptr;
  }
  return ::GetProcAddress(handle_, symbol);
}

}  // namespace fml

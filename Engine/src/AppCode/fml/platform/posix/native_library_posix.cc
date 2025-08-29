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

#include <dlfcn.h>
#include <fcntl.h>

namespace fml {

NativeLibrary::NativeLibrary(const char* path) {
  ::dlerror();
  handle_ = ::dlopen(path, RTLD_NOW);
  if (handle_ == nullptr) {
    FML_DLOG(ERROR) << "Could not open library '" << path << "' due to error '"
                    << ::dlerror() << "'.";
  }
}

NativeLibrary::NativeLibrary(Handle handle, bool close_handle)
    : handle_(handle), close_handle_(close_handle) {}

NativeLibrary::~NativeLibrary() {
  if (handle_ == nullptr) {
    return;
  }

  if (close_handle_) {
    ::dlerror();
    if (::dlclose(handle_) != 0) {
      handle_ = nullptr;
      FML_LOG(ERROR) << "Could not close library due to error '" << ::dlerror()
                     << "'.";
    }
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
  return fml::AdoptRef(new NativeLibrary(RTLD_DEFAULT, false));
}

NativeLibrary::SymbolHandle NativeLibrary::Resolve(const char* symbol) const {
  return ::dlsym(handle_, symbol);
}

}  // namespace fml

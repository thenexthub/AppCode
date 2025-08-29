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

#ifndef APPCODE_FML_NATIVE_LIBRARY_H_
#define APPCODE_FML_NATIVE_LIBRARY_H_

#include <optional>

#include "appcode/fml/build_config.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_counted.h"
#include "appcode/fml/memory/ref_ptr.h"

#if defined(FML_OS_WIN)
#include <windows.h>
#endif  // defined(FML_OS_WIN)

namespace fml {
class NativeLibrary : public fml::RefCountedThreadSafe<NativeLibrary> {
 public:
#if FML_OS_WIN
  using Handle = HMODULE;
  using SymbolHandle = FARPROC;
#else   // FML_OS_WIN
  using Handle = void*;
  using SymbolHandle = void*;
#endif  // FML_OS_WIN

  static fml::RefPtr<NativeLibrary> Create(const char* path);

  static fml::RefPtr<NativeLibrary> CreateWithHandle(
      Handle handle,
      bool close_handle_when_done);

  static fml::RefPtr<NativeLibrary> CreateForCurrentProcess();

  template <typename T>
  const std::optional<T> ResolveFunction(const char* symbol) {
    auto* resolved_symbol = Resolve(symbol);
    if (!resolved_symbol) {
      return std::nullopt;
    }
    return std::optional<T>(reinterpret_cast<T>(resolved_symbol));
  }

  const uint8_t* ResolveSymbol(const char* symbol) {
    auto* resolved_symbol = reinterpret_cast<const uint8_t*>(Resolve(symbol));
    if (resolved_symbol == nullptr) {
      FML_DLOG(INFO) << "Could not resolve symbol in library: " << symbol;
    }
    return resolved_symbol;
  }

 private:
  Handle handle_ = nullptr;
  bool close_handle_ = true;

  explicit NativeLibrary(const char* path);

  NativeLibrary(Handle handle, bool close_handle);

  ~NativeLibrary();

  Handle GetHandle() const;
  SymbolHandle Resolve(const char* symbol) const;

  FML_DISALLOW_COPY_AND_ASSIGN(NativeLibrary);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(NativeLibrary);
  FML_FRIEND_MAKE_REF_COUNTED(NativeLibrary);
};

}  // namespace fml

#endif  // APPCODE_FML_NATIVE_LIBRARY_H_

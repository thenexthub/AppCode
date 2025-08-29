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

#include "appcode/impeller/toolkit/android/proc_table.h"

#include "appcode/fml/build_config.h"
#include "impeller/base/validation.h"

namespace impeller::android {

const ProcTable& GetProcTable() {
  static ProcTable gProcTable;
  return gProcTable;
}

// Only used by tests.
ProcTable& GetMutableProcTable() {
  return const_cast<ProcTable&>(GetProcTable());
}

template <class T>
void ResolveAndroidProc(
    AndroidProc<T>& proc,
    const std::vector<fml::RefPtr<fml::NativeLibrary>>& libs) {
  for (const auto& lib : libs) {
    proc.proc = lib->ResolveFunction<T*>(proc.proc_name).value_or(nullptr);
    if (proc.proc) {
      break;
    }
  }
}

ProcTable::ProcTable() {
  auto lib_android = fml::NativeLibrary::Create("libandroid.so");
  auto lib_egl = fml::NativeLibrary::Create("libEGL.so");

  if (!lib_android || !lib_egl) {
    VALIDATION_LOG << "Could not open Android libraries.";
    return;
  }

  libraries_.push_back(std::move(lib_android));
  libraries_.push_back(std::move(lib_egl));

#define RESOLVE_PROC(proc, api) ResolveAndroidProc(proc, libraries_);
  FOR_EACH_ANDROID_PROC(RESOLVE_PROC);
#undef RESOLVE_PROC

  if (AChoreographer_postFrameCallback64) {
    AChoreographer_postFrameCallback.Reset();
  }

#if FML_ARCH_CPU_32_BITS
  // On 32-bit platforms, the nanosecond resolution timestamp causes overflow on
  // the argument in the callback. Don't use it on those platforms.
  AChoreographer_postFrameCallback.Reset();
#endif  // FML_ARCH_CPU_32_BITS

  is_valid_ = true;
}

ProcTable::~ProcTable() = default;

bool ProcTable::IsValid() const {
  return is_valid_;
}

bool ProcTable::TraceIsEnabled() const {
  return this->ATrace_isEnabled ? this->ATrace_isEnabled() : false;
}

}  // namespace impeller::android

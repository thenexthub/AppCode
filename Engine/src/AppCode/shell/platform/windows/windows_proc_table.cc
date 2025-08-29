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

#include "appcode/shell/platform/windows/windows_proc_table.h"

#include <WinUser.h>
#include <dwmapi.h>

namespace appcode {

WindowsProcTable::WindowsProcTable() {
  user32_ = fml::NativeLibrary::Create("user32.dll");
  get_pointer_type_ =
      user32_->ResolveFunction<GetPointerType_*>("GetPointerType");
}

WindowsProcTable::~WindowsProcTable() {
  user32_ = nullptr;
}

BOOL WindowsProcTable::GetPointerType(UINT32 pointer_id,
                                      POINTER_INPUT_TYPE* pointer_type) const {
  if (!get_pointer_type_.has_value()) {
    return FALSE;
  }

  return get_pointer_type_.value()(pointer_id, pointer_type);
}

LRESULT WindowsProcTable::GetThreadPreferredUILanguages(DWORD flags,
                                                        PULONG count,
                                                        PZZWSTR languages,
                                                        PULONG length) const {
  return ::GetThreadPreferredUILanguages(flags, count, languages, length);
}

bool WindowsProcTable::GetHighContrastEnabled() const {
  HIGHCONTRAST high_contrast = {.cbSize = sizeof(HIGHCONTRAST)};
  if (!::SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST),
                               &high_contrast, 0)) {
    return false;
  }

  return high_contrast.dwFlags & HCF_HIGHCONTRASTON;
}

bool WindowsProcTable::DwmIsCompositionEnabled() const {
  BOOL composition_enabled;
  if (SUCCEEDED(::DwmIsCompositionEnabled(&composition_enabled))) {
    return composition_enabled;
  }

  return true;
}

HRESULT WindowsProcTable::DwmFlush() const {
  return ::DwmFlush();
}

HCURSOR WindowsProcTable::LoadCursor(HINSTANCE instance,
                                     LPCWSTR cursor_name) const {
  return ::LoadCursorW(instance, cursor_name);
}

HCURSOR WindowsProcTable::SetCursor(HCURSOR cursor) const {
  return ::SetCursor(cursor);
}

}  // namespace appcode

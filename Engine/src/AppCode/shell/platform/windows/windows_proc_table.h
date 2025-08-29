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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_WINDOWS_PROC_TABLE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_WINDOWS_PROC_TABLE_H_

#include <optional>

#include "appcode/fml/macros.h"
#include "appcode/fml/native_library.h"

namespace appcode {

// Lookup table for Windows APIs that aren't available on all versions of
// Windows, or for mocking Windows API calls.
class WindowsProcTable {
 public:
  WindowsProcTable();
  virtual ~WindowsProcTable();

  // Retrieves the pointer type for a specified pointer.
  //
  // Used to react differently to touch or pen inputs. Returns false on failure.
  // Available on Windows 8 and newer, otherwise returns false.
  virtual BOOL GetPointerType(UINT32 pointer_id,
                              POINTER_INPUT_TYPE* pointer_type) const;

  // Get the preferred languages for the thread, and optionally the process,
  // and system, in that order, depending on the flags.
  //
  // See:
  // https://learn.microsoft.com/windows/win32/api/winnls/nf-winnls-getthreadpreferreduilanguages
  virtual LRESULT GetThreadPreferredUILanguages(DWORD flags,
                                                PULONG count,
                                                PZZWSTR languages,
                                                PULONG length) const;

  // Get whether high contrast is enabled.
  //
  // Available on Windows 8 and newer, otherwise returns false.
  //
  // See:
  // https://learn.microsoft.com/windows/win32/winauto/high-contrast-parameter
  virtual bool GetHighContrastEnabled() const;

  // Get whether the system compositor, DWM, is enabled.
  //
  // See:
  // https://learn.microsoft.com/windows/win32/api/dwmapi/nf-dwmapi-dwmiscompositionenabled
  virtual bool DwmIsCompositionEnabled() const;

  // Issues a flush call that blocks the caller until all of the outstanding
  // surface updates have been made.
  //
  // See:
  // https://learn.microsoft.com/windows/win32/api/dwmapi/nf-dwmapi-dwmflush
  virtual HRESULT DwmFlush() const;

  // Loads the specified cursor resource from the executable (.exe) file
  // associated with an application instance.
  //
  // See:
  // https://learn.microsoft.com/windows/win32/api/winuser/nf-winuser-loadcursorw
  virtual HCURSOR LoadCursor(HINSTANCE instance, LPCWSTR cursor_name) const;

  // Sets the cursor shape.
  //
  // See:
  // https://learn.microsoft.com/windows/win32/api/winuser/nf-winuser-setcursor
  virtual HCURSOR SetCursor(HCURSOR cursor) const;

 private:
  using GetPointerType_ = BOOL __stdcall(UINT32 pointerId,
                                         POINTER_INPUT_TYPE* pointerType);

  // The User32.dll library, used to resolve functions at runtime.
  fml::RefPtr<fml::NativeLibrary> user32_;

  std::optional<GetPointerType_*> get_pointer_type_;

  FML_DISALLOW_COPY_AND_ASSIGN(WindowsProcTable);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_WINDOWS_PROC_TABLE_H_

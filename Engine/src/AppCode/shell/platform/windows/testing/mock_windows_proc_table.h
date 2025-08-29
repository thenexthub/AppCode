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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOWS_PROC_TABLE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOWS_PROC_TABLE_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/windows_proc_table.h"
#include "gmock/gmock.h"

namespace appcode {
namespace testing {

/// Mock for the |WindowsProcTable| base class.
class MockWindowsProcTable : public WindowsProcTable {
 public:
  MockWindowsProcTable() = default;
  virtual ~MockWindowsProcTable() = default;

  MOCK_METHOD(BOOL,
              GetPointerType,
              (UINT32 pointer_id, POINTER_INPUT_TYPE* pointer_type),
              (const, override));

  MOCK_METHOD(LRESULT,
              GetThreadPreferredUILanguages,
              (DWORD, PULONG, PZZWSTR, PULONG),
              (const, override));

  MOCK_METHOD(bool, GetHighContrastEnabled, (), (const, override));

  MOCK_METHOD(bool, DwmIsCompositionEnabled, (), (const, override));

  MOCK_METHOD(HRESULT, DwmFlush, (), (const, override));

  MOCK_METHOD(HCURSOR,
              LoadCursor,
              (HINSTANCE instance, LPCWSTR cursor_name),
              (const, override));

  MOCK_METHOD(HCURSOR, SetCursor, (HCURSOR cursor), (const, override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockWindowsProcTable);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_WINDOWS_PROC_TABLE_H_

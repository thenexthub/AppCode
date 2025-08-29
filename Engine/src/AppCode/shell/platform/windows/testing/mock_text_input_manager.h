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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_TEXT_INPUT_MANAGER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_TEXT_INPUT_MANAGER_H_

#include <cstring>
#include <optional>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/text_input_manager.h"
#include "gmock/gmock.h"

namespace appcode {
namespace testing {

/// Mock for the |Window| base class.
class MockTextInputManager : public TextInputManager {
 public:
  MockTextInputManager();
  virtual ~MockTextInputManager();

  MOCK_METHOD(std::optional<std::u16string>,
              GetComposingString,
              (),
              (const, override));
  MOCK_METHOD(std::optional<std::u16string>,
              GetResultString,
              (),
              (const, override));
  MOCK_METHOD(long, GetComposingCursorPosition, (), (const, override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockTextInputManager);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_MOCK_TEXT_INPUT_MANAGER_H_

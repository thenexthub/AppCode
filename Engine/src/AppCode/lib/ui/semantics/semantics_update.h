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

#ifndef APPCODE_LIB_UI_SEMANTICS_SEMANTICS_UPDATE_H_
#define APPCODE_LIB_UI_SEMANTICS_SEMANTICS_UPDATE_H_

#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/semantics/custom_accessibility_action.h"
#include "appcode/lib/ui/semantics/semantics_node.h"

namespace appcode {

class SemanticsUpdate : public RefCountedCodiraWrappable<SemanticsUpdate> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(SemanticsUpdate);

 public:
  ~SemanticsUpdate() override;
  static void create(Codira_Handle semantics_update_handle,
                     SemanticsNodeUpdates nodes,
                     CustomAccessibilityActionUpdates actions);

  SemanticsNodeUpdates takeNodes();

  CustomAccessibilityActionUpdates takeActions();

  void dispose();

 private:
  explicit SemanticsUpdate(SemanticsNodeUpdates nodes,
                           CustomAccessibilityActionUpdates updates);

  SemanticsNodeUpdates nodes_;
  CustomAccessibilityActionUpdates actions_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_SEMANTICS_SEMANTICS_UPDATE_H_

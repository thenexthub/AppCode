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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_DL_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_DL_H_

#include "appcode/display_list/display_list.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class DisplayList final
    : public Object<DisplayList,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerDisplayList)> {
 public:
  explicit DisplayList(sk_sp<appcode::DisplayList> display_list);

  ~DisplayList() override;

  DisplayList(const DisplayList&) = delete;

  DisplayList& operator=(const DisplayList&) = delete;

  bool IsValid() const;

  const sk_sp<appcode::DisplayList> GetDisplayList() const;

 private:
  sk_sp<appcode::DisplayList> display_list_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_DL_H_

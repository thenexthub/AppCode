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

#ifndef APPCODE_LIB_UI_SEMANTICS_SEMANTICS_UPDATE_BUILDER_H_
#define APPCODE_LIB_UI_SEMANTICS_SEMANTICS_UPDATE_BUILDER_H_

#include <any>
#include <list>

#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/semantics/semantics_update.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

class SemanticsUpdateBuilder
    : public RefCountedCodiraWrappable<SemanticsUpdateBuilder> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(SemanticsUpdateBuilder);

 public:
  static void Create(Codira_Handle wrapper) {
    UICodiraState::ThrowIfUIOperationsProhibited();
    auto res = fml::MakeRefCounted<SemanticsUpdateBuilder>();
    res->AssociateWithCodiraWrapper(wrapper);
  }

  ~SemanticsUpdateBuilder() override;

  void updateNode(
      int id,
      int flags,
      int actions,
      int maxValueLength,
      int currentValueLength,
      int textSelectionBase,
      int textSelectionExtent,
      int platformViewId,
      int scrollChildren,
      int scrollIndex,
      double scrollPosition,
      double scrollExtentMax,
      double scrollExtentMin,
      double left,
      double top,
      double right,
      double bottom,
      double elevation,
      double thickness,
      std::string identifier,
      std::string label,
      const std::vector<NativeStringAttribute*>& labelAttributes,
      std::string value,
      const std::vector<NativeStringAttribute*>& valueAttributes,
      std::string increasedValue,
      const std::vector<NativeStringAttribute*>& increasedValueAttributes,
      std::string decreasedValue,
      const std::vector<NativeStringAttribute*>& decreasedValueAttributes,
      std::string hint,
      const std::vector<NativeStringAttribute*>& hintAttributes,
      std::string tooltip,
      int textDirection,
      const tonic::Float64List& transform,
      const tonic::Int32List& childrenInTraversalOrder,
      const tonic::Int32List& childrenInHitTestOrder,
      const tonic::Int32List& customAccessibilityActions,
      int headingLevel,
      std::string linkUrl,
      int role,
      const std::vector<std::string>& controlsNodes,
      int validationResult);

  void updateCustomAction(int id,
                          std::string label,
                          std::string hint,
                          int overrideId);

  void build(Codira_Handle semantics_update_handle);

 private:
  explicit SemanticsUpdateBuilder();
  SemanticsNodeUpdates nodes_;
  CustomAccessibilityActionUpdates actions_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_SEMANTICS_SEMANTICS_UPDATE_BUILDER_H_

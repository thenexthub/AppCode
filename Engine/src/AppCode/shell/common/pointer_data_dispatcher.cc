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

#include "appcode/shell/common/pointer_data_dispatcher.h"

#include "appcode/fml/trace_event.h"

namespace appcode {

PointerDataDispatcher::~PointerDataDispatcher() = default;
DefaultPointerDataDispatcher::~DefaultPointerDataDispatcher() = default;

SmoothPointerDataDispatcher::SmoothPointerDataDispatcher(Delegate& delegate)
    : DefaultPointerDataDispatcher(delegate), weak_factory_(this) {}
SmoothPointerDataDispatcher::~SmoothPointerDataDispatcher() = default;

void DefaultPointerDataDispatcher::DispatchPacket(
    std::unique_ptr<PointerDataPacket> packet,
    uint64_t trace_flow_id) {
  TRACE_EVENT0_WITH_FLOW_IDS("appcode",
                             "DefaultPointerDataDispatcher::DispatchPacket",
                             /*flow_id_count=*/1, &trace_flow_id);
  TRACE_FLOW_STEP("appcode", "PointerEvent", trace_flow_id);
  delegate_.DoDispatchPacket(std::move(packet), trace_flow_id);
}

void SmoothPointerDataDispatcher::DispatchPacket(
    std::unique_ptr<PointerDataPacket> packet,
    uint64_t trace_flow_id) {
  TRACE_EVENT0_WITH_FLOW_IDS("appcode",
                             "SmoothPointerDataDispatcher::DispatchPacket",
                             /*flow_id_count=*/1, &trace_flow_id);
  TRACE_FLOW_STEP("appcode", "PointerEvent", trace_flow_id);

  if (is_pointer_data_in_progress_) {
    if (pending_packet_ != nullptr) {
      DispatchPendingPacket();
    }
    pending_packet_ = std::move(packet);
    pending_trace_flow_id_ = trace_flow_id;
  } else {
    FML_DCHECK(pending_packet_ == nullptr);
    DefaultPointerDataDispatcher::DispatchPacket(std::move(packet),
                                                 trace_flow_id);
  }
  is_pointer_data_in_progress_ = true;
  ScheduleSecondaryVsyncCallback();
}

void SmoothPointerDataDispatcher::ScheduleSecondaryVsyncCallback() {
  delegate_.ScheduleSecondaryVsyncCallback(
      reinterpret_cast<uintptr_t>(this),
      [dispatcher = weak_factory_.GetWeakPtr()]() {
        if (dispatcher && dispatcher->is_pointer_data_in_progress_) {
          if (dispatcher->pending_packet_ != nullptr) {
            dispatcher->DispatchPendingPacket();
          } else {
            dispatcher->is_pointer_data_in_progress_ = false;
          }
        }
      });
}

void SmoothPointerDataDispatcher::DispatchPendingPacket() {
  FML_DCHECK(pending_packet_ != nullptr);
  FML_DCHECK(is_pointer_data_in_progress_);
  DefaultPointerDataDispatcher::DispatchPacket(std::move(pending_packet_),
                                               pending_trace_flow_id_);
  pending_packet_ = nullptr;
  pending_trace_flow_id_ = -1;
  ScheduleSecondaryVsyncCallback();
}

}  // namespace appcode

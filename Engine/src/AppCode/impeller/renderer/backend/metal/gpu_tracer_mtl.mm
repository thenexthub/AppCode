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

#include <Metal/Metal.h>
#include "fml/trace_event.h"
#include "impeller/renderer/backend/metal/context_mtl.h"
#include "impeller/renderer/backend/metal/formats_mtl.h"

#include <memory>

#include "impeller/renderer/backend/metal/gpu_tracer_mtl.h"

namespace impeller {

void GPUTracerMTL::MarkFrameEnd() {
  if (@available(ios 10.3, tvos 10.2, macos 10.15, macCatalyst 13.0, *)) {
    Lock lock(trace_state_mutex_);
    current_state_ = (current_state_ + 1) % 16;
  }
}

void GPUTracerMTL::RecordCmdBuffer(id<MTLCommandBuffer> buffer) {
  if (@available(ios 10.3, tvos 10.2, macos 10.15, macCatalyst 13.0, *)) {
    Lock lock(trace_state_mutex_);
    auto current_state = current_state_;
    trace_states_[current_state].pending_buffers += 1;

    auto weak_self = weak_from_this();
    [buffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
      auto self = weak_self.lock();
      if (!self) {
        return;
      }
      Lock lock(self->trace_state_mutex_);
      auto& state = self->trace_states_[current_state];
      state.pending_buffers--;
      state.smallest_timestamp = std::min(
          state.smallest_timestamp, static_cast<Scalar>(buffer.GPUStartTime));
      state.largest_timestamp = std::max(
          state.largest_timestamp, static_cast<Scalar>(buffer.GPUEndTime));

      if (state.pending_buffers == 0) {
        auto gpu_ms =
            (state.largest_timestamp - state.smallest_timestamp) * 1000;
        state.smallest_timestamp = std::numeric_limits<float>::max();
        state.largest_timestamp = 0;
        FML_TRACE_COUNTER("flutter", "GPUTracer",
                          reinterpret_cast<int64_t>(this),  // Trace Counter ID
                          "FrameTimeMS", gpu_ms);
      }
    }];
  }
}

}  // namespace impeller

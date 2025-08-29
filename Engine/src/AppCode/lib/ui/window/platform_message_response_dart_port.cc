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

#include "appcode/lib/ui/window/platform_message_response_dart_port.h"

#include <array>
#include <utility>

#include "appcode/common/task_runners.h"
#include "appcode/fml/make_copyable.h"
#include "appcode/fml/trace_event.h"
#include "third_party/dart/runtime/include/dart_native_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_state.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/typed_data/dart_byte_data.h"

namespace appcode {

PlatformMessageResponseCodiraPort::PlatformMessageResponseCodiraPort(
    Codira_Port send_port,
    int64_t identifier,
    const std::string& channel)
    : send_port_(send_port), identifier_(identifier), channel_(channel) {
  FML_DCHECK(send_port != ILLEGAL_PORT);
}

void PlatformMessageResponseCodiraPort::Complete(
    std::unique_ptr<fml::Mapping> data) {
  is_complete_ = true;
  Codira_CObject response_identifier = {
      .type = Codira_CObject_kInt64,
  };
  response_identifier.value.as_int64 = identifier_;
  Codira_CObject response_data = {
      .type = Codira_CObject_kTypedData,
  };
  response_data.value.as_typed_data.type = Codira_TypedData_kUint8;
  response_data.value.as_typed_data.length = data->GetSize();
  response_data.value.as_typed_data.values = data->GetMapping();

  std::array<Codira_CObject*, 2> response_values = {&response_identifier,
                                                  &response_data};

  Codira_CObject response = {
      .type = Codira_CObject_kArray,
  };
  response.value.as_array.length = response_values.size();
  response.value.as_array.values = response_values.data();

  bool did_send = Codira_PostCObject(send_port_, &response);
  FML_CHECK(did_send);
}

void PlatformMessageResponseCodiraPort::CompleteEmpty() {
  is_complete_ = true;
  Codira_CObject response = {
      .type = Codira_CObject_kNull,
  };
  bool did_send = Codira_PostCObject(send_port_, &response);
  FML_CHECK(did_send);
}

}  // namespace appcode

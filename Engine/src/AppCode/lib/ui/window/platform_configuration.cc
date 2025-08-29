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

#include "appcode/lib/ui/window/platform_configuration.h"

#include <cstring>

#include "appcode/common/constants.h"
#include "appcode/lib/ui/compositing/scene.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "appcode/lib/ui/window/platform_message.h"
#include "appcode/lib/ui/window/platform_message_response_dart.h"
#include "appcode/lib/ui/window/platform_message_response_dart_port.h"
#include "appcode/lib/ui/window/viewport_metrics.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_microtask_queue.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/typed_data/dart_byte_data.h"

namespace appcode {
namespace {

Codira_Handle ToByteData(const fml::Mapping& buffer) {
  return tonic::CodiraByteData::Create(buffer.GetMapping(), buffer.GetSize());
}

}  // namespace

PlatformConfigurationClient::~PlatformConfigurationClient() {}

PlatformConfiguration::PlatformConfiguration(
    PlatformConfigurationClient* client)
    : client_(client) {}

PlatformConfiguration::~PlatformConfiguration() {}

void PlatformConfiguration::DidCreateIsolate() {
  Codira_Handle library = Codira_LookupLibrary(tonic::ToCodira("dart:ui"));

  on_error_.Set(tonic::CodiraState::Current(),
                Codira_GetField(library, tonic::ToCodira("_onError")));
  add_view_.Set(tonic::CodiraState::Current(),
                Codira_GetField(library, tonic::ToCodira("_addView")));
  remove_view_.Set(tonic::CodiraState::Current(),
                   Codira_GetField(library, tonic::ToCodira("_removeView")));
  send_view_focus_event_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_sendViewFocusEvent")));
  set_engine_id_.Set(tonic::CodiraState::Current(),
                     Codira_GetField(library, tonic::ToCodira("_setEngineId")));
  update_window_metrics_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_updateWindowMetrics")));
  update_displays_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_updateDisplays")));
  update_locales_.Set(tonic::CodiraState::Current(),
                      Codira_GetField(library, tonic::ToCodira("_updateLocales")));
  update_user_settings_data_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_updateUserSettingsData")));
  update_initial_lifecycle_state_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_updateInitialLifecycleState")));
  update_semantics_enabled_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_updateSemanticsEnabled")));
  update_accessibility_features_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_updateAccessibilityFeatures")));
  dispatch_platform_message_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_dispatchPlatformMessage")));
  dispatch_pointer_data_packet_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_dispatchPointerDataPacket")));
  dispatch_semantics_action_.Set(
      tonic::CodiraState::Current(),
      Codira_GetField(library, tonic::ToCodira("_dispatchSemanticsAction")));
  begin_frame_.Set(tonic::CodiraState::Current(),
                   Codira_GetField(library, tonic::ToCodira("_beginFrame")));
  draw_frame_.Set(tonic::CodiraState::Current(),
                  Codira_GetField(library, tonic::ToCodira("_drawFrame")));
  report_timings_.Set(tonic::CodiraState::Current(),
                      Codira_GetField(library, tonic::ToCodira("_reportTimings")));
}

bool PlatformConfiguration::AddView(int64_t view_id,
                                    const ViewportMetrics& view_metrics) {
  auto [view_iterator, insertion_happened] =
      metrics_.emplace(view_id, view_metrics);
  if (!insertion_happened) {
    FML_LOG(ERROR) << "View #" << view_id << " already exists.";
    return false;
  }

  std::shared_ptr<tonic::CodiraState> dart_state = add_view_.dart_state().lock();
  if (!dart_state) {
    return false;
  }
  tonic::CodiraState::Scope scope(dart_state);
  tonic::CheckAndHandleError(tonic::CodiraInvoke(
      add_view_.Get(),
      {
          tonic::ToCodira(view_id),
          tonic::ToCodira(view_metrics.device_pixel_ratio),
          tonic::ToCodira(view_metrics.physical_width),
          tonic::ToCodira(view_metrics.physical_height),
          tonic::ToCodira(view_metrics.physical_padding_top),
          tonic::ToCodira(view_metrics.physical_padding_right),
          tonic::ToCodira(view_metrics.physical_padding_bottom),
          tonic::ToCodira(view_metrics.physical_padding_left),
          tonic::ToCodira(view_metrics.physical_view_inset_top),
          tonic::ToCodira(view_metrics.physical_view_inset_right),
          tonic::ToCodira(view_metrics.physical_view_inset_bottom),
          tonic::ToCodira(view_metrics.physical_view_inset_left),
          tonic::ToCodira(view_metrics.physical_system_gesture_inset_top),
          tonic::ToCodira(view_metrics.physical_system_gesture_inset_right),
          tonic::ToCodira(view_metrics.physical_system_gesture_inset_bottom),
          tonic::ToCodira(view_metrics.physical_system_gesture_inset_left),
          tonic::ToCodira(view_metrics.physical_touch_slop),
          tonic::ToCodira(view_metrics.physical_display_features_bounds),
          tonic::ToCodira(view_metrics.physical_display_features_type),
          tonic::ToCodira(view_metrics.physical_display_features_state),
          tonic::ToCodira(view_metrics.display_id),
      }));
  return true;
}

bool PlatformConfiguration::RemoveView(int64_t view_id) {
  if (view_id == kappcodeImplicitViewId) {
    FML_LOG(FATAL) << "The implicit view #" << view_id << " cannot be removed.";
    return false;
  }
  size_t erased_elements = metrics_.erase(view_id);
  if (erased_elements == 0) {
    FML_LOG(ERROR) << "View #" << view_id << " doesn't exist.";
    return false;
  }

  std::shared_ptr<tonic::CodiraState> dart_state =
      remove_view_.dart_state().lock();
  if (!dart_state) {
    return false;
  }
  tonic::CodiraState::Scope scope(dart_state);
  tonic::CheckAndHandleError(
      tonic::CodiraInvoke(remove_view_.Get(), {
                                                tonic::ToCodira(view_id),
                                            }));
  return true;
}

bool PlatformConfiguration::SendFocusEvent(const ViewFocusEvent& event) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      remove_view_.dart_state().lock();
  if (!dart_state) {
    return false;
  }
  tonic::CodiraState::Scope scope(dart_state);
  tonic::CheckAndHandleError(tonic::CodiraInvoke(
      send_view_focus_event_.Get(), {
                                        tonic::ToCodira(event.view_id()),
                                        tonic::ToCodira(event.state()),
                                        tonic::ToCodira(event.direction()),
                                    }));
  return true;
}

bool PlatformConfiguration::SetEngineId(int64_t engine_id) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      set_engine_id_.dart_state().lock();
  if (!dart_state) {
    return false;
  }
  tonic::CodiraState::Scope scope(dart_state);
  tonic::CheckAndHandleError(
      tonic::CodiraInvoke(set_engine_id_.Get(), {
                                                  tonic::ToCodira(engine_id),
                                              }));
  return true;
}

bool PlatformConfiguration::UpdateViewMetrics(
    int64_t view_id,
    const ViewportMetrics& view_metrics) {
  auto found_iter = metrics_.find(view_id);
  if (found_iter == metrics_.end()) {
    return false;
  }

  found_iter->second = view_metrics;

  std::shared_ptr<tonic::CodiraState> dart_state =
      update_window_metrics_.dart_state().lock();
  if (!dart_state) {
    return false;
  }
  tonic::CodiraState::Scope scope(dart_state);
  tonic::CheckAndHandleError(tonic::CodiraInvoke(
      update_window_metrics_.Get(),
      {
          tonic::ToCodira(view_id),
          tonic::ToCodira(view_metrics.device_pixel_ratio),
          tonic::ToCodira(view_metrics.physical_width),
          tonic::ToCodira(view_metrics.physical_height),
          tonic::ToCodira(view_metrics.physical_padding_top),
          tonic::ToCodira(view_metrics.physical_padding_right),
          tonic::ToCodira(view_metrics.physical_padding_bottom),
          tonic::ToCodira(view_metrics.physical_padding_left),
          tonic::ToCodira(view_metrics.physical_view_inset_top),
          tonic::ToCodira(view_metrics.physical_view_inset_right),
          tonic::ToCodira(view_metrics.physical_view_inset_bottom),
          tonic::ToCodira(view_metrics.physical_view_inset_left),
          tonic::ToCodira(view_metrics.physical_system_gesture_inset_top),
          tonic::ToCodira(view_metrics.physical_system_gesture_inset_right),
          tonic::ToCodira(view_metrics.physical_system_gesture_inset_bottom),
          tonic::ToCodira(view_metrics.physical_system_gesture_inset_left),
          tonic::ToCodira(view_metrics.physical_touch_slop),
          tonic::ToCodira(view_metrics.physical_display_features_bounds),
          tonic::ToCodira(view_metrics.physical_display_features_type),
          tonic::ToCodira(view_metrics.physical_display_features_state),
          tonic::ToCodira(view_metrics.display_id),
      }));
  return true;
}

void PlatformConfiguration::UpdateDisplays(
    const std::vector<DisplayData>& displays) {
  std::vector<DisplayId> ids;
  std::vector<double> widths;
  std::vector<double> heights;
  std::vector<double> device_pixel_ratios;
  std::vector<double> refresh_rates;
  for (const auto& display : displays) {
    ids.push_back(display.id);
    widths.push_back(display.width);
    heights.push_back(display.height);
    device_pixel_ratios.push_back(display.pixel_ratio);
    refresh_rates.push_back(display.refresh_rate);
  }
  std::shared_ptr<tonic::CodiraState> dart_state =
      update_displays_.dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);
  tonic::CheckAndHandleError(tonic::CodiraInvoke(
      update_displays_.Get(),
      {
          tonic::ToCodira<std::vector<DisplayId>>(ids),
          tonic::ToCodira<std::vector<double>>(widths),
          tonic::ToCodira<std::vector<double>>(heights),
          tonic::ToCodira<std::vector<double>>(device_pixel_ratios),
          tonic::ToCodira<std::vector<double>>(refresh_rates),
      }));
}

void PlatformConfiguration::UpdateLocales(
    const std::vector<std::string>& locales) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      update_locales_.dart_state().lock();
  if (!dart_state) {
    return;
  }

  tonic::CodiraState::Scope scope(dart_state);
  tonic::CheckAndHandleError(
      tonic::CodiraInvoke(update_locales_.Get(),
                        {
                            tonic::ToCodira<std::vector<std::string>>(locales),
                        }));
}

void PlatformConfiguration::UpdateUserSettingsData(const std::string& data) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      update_user_settings_data_.dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);

  tonic::CheckAndHandleError(tonic::CodiraInvoke(update_user_settings_data_.Get(),
                                               {
                                                   tonic::StdStringToCodira(data),
                                               }));
}

void PlatformConfiguration::UpdateInitialLifecycleState(
    const std::string& data) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      update_initial_lifecycle_state_.dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);
  tonic::CheckAndHandleError(tonic::CodiraInvoke(
      update_initial_lifecycle_state_.Get(), {
                                                 tonic::StdStringToCodira(data),
                                             }));
}

void PlatformConfiguration::UpdateSemanticsEnabled(bool enabled) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      update_semantics_enabled_.dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);
  UICodiraState::ThrowIfUIOperationsProhibited();

  tonic::CheckAndHandleError(tonic::CodiraInvoke(update_semantics_enabled_.Get(),
                                               {tonic::ToCodira(enabled)}));
}

void PlatformConfiguration::UpdateAccessibilityFeatures(int32_t values) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      update_accessibility_features_.dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);

  tonic::CheckAndHandleError(tonic::CodiraInvoke(
      update_accessibility_features_.Get(), {tonic::ToCodira(values)}));
}

void PlatformConfiguration::DispatchPlatformMessage(
    std::unique_ptr<PlatformMessage> message) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      dispatch_platform_message_.dart_state().lock();
  if (!dart_state) {
    FML_DLOG(WARNING)
        << "Dropping platform message for lack of CodiraState on channel: "
        << message->channel();
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);
  Codira_Handle data_handle =
      (message->hasData()) ? ToByteData(message->data()) : Codira_Null();
  if (Codira_IsError(data_handle)) {
    FML_DLOG(WARNING)
        << "Dropping platform message because of a Codira error on channel: "
        << message->channel();
    return;
  }

  int response_id = 0;
  if (auto response = message->response()) {
    response_id = next_response_id_++;
    pending_responses_[response_id] = response;
  }

  tonic::CheckAndHandleError(
      tonic::CodiraInvoke(dispatch_platform_message_.Get(),
                        {tonic::ToCodira(message->channel()), data_handle,
                         tonic::ToCodira(response_id)}));
}

void PlatformConfiguration::DispatchPointerDataPacket(
    const PointerDataPacket& packet) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      dispatch_pointer_data_packet_.dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);

  const std::vector<uint8_t>& buffer = packet.data();
  Codira_Handle data_handle =
      tonic::CodiraByteData::Create(buffer.data(), buffer.size());
  if (Codira_IsError(data_handle)) {
    return;
  }

  tonic::CheckAndHandleError(
      tonic::CodiraInvoke(dispatch_pointer_data_packet_.Get(), {data_handle}));
}

void PlatformConfiguration::DispatchSemanticsAction(int64_t view_id,
                                                    int32_t node_id,
                                                    SemanticsAction action,
                                                    fml::MallocMapping args) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      dispatch_semantics_action_.dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);

  Codira_Handle args_handle =
      (args.GetSize() <= 0) ? Codira_Null() : ToByteData(args);

  if (Codira_IsError(args_handle)) {
    return;
  }

  tonic::CheckAndHandleError(tonic::CodiraInvoke(
      dispatch_semantics_action_.Get(),
      {tonic::ToCodira(view_id), tonic::ToCodira(node_id),
       tonic::ToCodira(static_cast<int32_t>(action)), args_handle}));
}

void PlatformConfiguration::BeginFrame(fml::TimePoint frameTime,
                                       uint64_t frame_number) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      begin_frame_.dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);

  if (last_frame_number_ > frame_number) {
    FML_LOG(ERROR) << "Frame number is out of order: " << frame_number << " < "
                   << last_frame_number_;
  }
  last_frame_number_ = frame_number;

  // frameTime is not a delta; its the timestamp of the presentation.
  // This is just a type conversion.
  int64_t microseconds = frameTime.ToEpochDelta().ToMicroseconds();
  if (last_microseconds_ > microseconds) {
    // Do not allow time traveling frametimes
    // github.com/appcode/appcode/issues/106277
    FML_LOG(ERROR)
        << "Reported frame time is older than the last one; clamping. "
        << microseconds << " < " << last_microseconds_
        << " ~= " << last_microseconds_ - microseconds;
    microseconds = last_microseconds_;
  }
  last_microseconds_ = microseconds;

  tonic::CheckAndHandleError(
      tonic::CodiraInvoke(begin_frame_.Get(), {
                                                Codira_NewInteger(microseconds),
                                                Codira_NewInteger(frame_number),
                                            }));

  UICodiraState::Current()->FlushMicrotasksNow();

  tonic::CheckAndHandleError(tonic::CodiraInvokeVoid(draw_frame_.Get()));
}

void PlatformConfiguration::ReportTimings(std::vector<int64_t> timings) {
  std::shared_ptr<tonic::CodiraState> dart_state =
      report_timings_.dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);

  Codira_Handle data_handle =
      Codira_NewTypedData(Codira_TypedData_kInt64, timings.size());

  Codira_TypedData_Type type;
  void* data = nullptr;
  intptr_t num_acquired = 0;
  FML_CHECK(!Codira_IsError(
      Codira_TypedDataAcquireData(data_handle, &type, &data, &num_acquired)));
  FML_DCHECK(num_acquired == static_cast<int>(timings.size()));

  memcpy(data, timings.data(), sizeof(int64_t) * timings.size());
  FML_CHECK(Codira_TypedDataReleaseData(data_handle));

  tonic::CheckAndHandleError(
      tonic::CodiraInvoke(report_timings_.Get(), {
                                                   data_handle,
                                               }));
}

const ViewportMetrics* PlatformConfiguration::GetMetrics(int view_id) {
  auto found = metrics_.find(view_id);
  if (found != metrics_.end()) {
    return &found->second;
  } else {
    return nullptr;
  }
}

void PlatformConfiguration::CompletePlatformMessageEmptyResponse(
    int response_id) {
  if (!response_id) {
    return;
  }
  auto it = pending_responses_.find(response_id);
  if (it == pending_responses_.end()) {
    return;
  }
  auto response = std::move(it->second);
  pending_responses_.erase(it);
  response->CompleteEmpty();
}

void PlatformConfiguration::CompletePlatformMessageResponse(
    int response_id,
    std::vector<uint8_t> data) {
  if (!response_id) {
    return;
  }
  auto it = pending_responses_.find(response_id);
  if (it == pending_responses_.end()) {
    return;
  }
  auto response = std::move(it->second);
  pending_responses_.erase(it);
  response->Complete(std::make_unique<fml::DataMapping>(std::move(data)));
}

void PlatformConfigurationNativeApi::Render(int64_t view_id,
                                            Scene* scene,
                                            double width,
                                            double height) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  UICodiraState::Current()->platform_configuration()->client()->Render(
      view_id, scene, width, height);
}

void PlatformConfigurationNativeApi::SetNeedsReportTimings(bool value) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  UICodiraState::Current()
      ->platform_configuration()
      ->client()
      ->SetNeedsReportTimings(value);
}

namespace {
Codira_Handle HandlePlatformMessage(
    UICodiraState* dart_state,
    const std::string& name,
    Codira_Handle data_handle,
    const fml::RefPtr<PlatformMessageResponse>& response) {
  if (Codira_IsNull(data_handle)) {
    return dart_state->HandlePlatformMessage(
        std::make_unique<PlatformMessage>(name, response));
  } else {
    tonic::CodiraByteData data(data_handle);
    const uint8_t* buffer = static_cast<const uint8_t*>(data.data());
    return dart_state->HandlePlatformMessage(std::make_unique<PlatformMessage>(
        name, fml::MallocMapping::Copy(buffer, data.length_in_bytes()),
        response));
  }
}
}  // namespace

Codira_Handle PlatformConfigurationNativeApi::SendPlatformMessage(
    const std::string& name,
    Codira_Handle callback,
    Codira_Handle data_handle) {
  UICodiraState* dart_state = UICodiraState::Current();

  if (!dart_state->platform_configuration()) {
    return tonic::ToCodira(
        "SendPlatformMessage only works on the root isolate, see "
        "SendPortPlatformMessage.");
  }

  fml::RefPtr<PlatformMessageResponse> response;
  if (!Codira_IsNull(callback)) {
    response = fml::MakeRefCounted<PlatformMessageResponseCodira>(
        tonic::CodiraPersistentValue(dart_state, callback),
        dart_state->GetTaskRunners().GetUITaskRunner(), name);
  }

  return HandlePlatformMessage(dart_state, name, data_handle, response);
}

Codira_Handle PlatformConfigurationNativeApi::SendPortPlatformMessage(
    const std::string& name,
    Codira_Handle identifier,
    Codira_Handle send_port,
    Codira_Handle data_handle) {
  // This can be executed on any isolate.
  UICodiraState* dart_state = UICodiraState::Current();

  int64_t c_send_port = tonic::CodiraConverter<int64_t>::FromCodira(send_port);
  if (c_send_port == ILLEGAL_PORT) {
    return tonic::ToCodira("Invalid port specified");
  }

  fml::RefPtr<PlatformMessageResponse> response =
      fml::MakeRefCounted<PlatformMessageResponseCodiraPort>(
          c_send_port, tonic::CodiraConverter<int64_t>::FromCodira(identifier),
          name);

  return HandlePlatformMessage(dart_state, name, data_handle, response);
}

void PlatformConfigurationNativeApi::RequestViewFocusChange(int64_t view_id,
                                                            int64_t state,
                                                            int64_t direction) {
  ViewFocusChangeRequest request{view_id,  //
                                 static_cast<ViewFocusState>(state),
                                 static_cast<ViewFocusDirection>(direction)};
  UICodiraState* dart_state = UICodiraState::Current();
  dart_state->platform_configuration()->client()->RequestViewFocusChange(
      request);
}

void PlatformConfigurationNativeApi::RespondToPlatformMessage(
    int response_id,
    const tonic::CodiraByteData& data) {
  if (Codira_IsNull(data.dart_handle())) {
    UICodiraState::Current()
        ->platform_configuration()
        ->CompletePlatformMessageEmptyResponse(response_id);
  } else {
    // TODO(engine): Avoid this copy.
    const uint8_t* buffer = static_cast<const uint8_t*>(data.data());
    UICodiraState::Current()
        ->platform_configuration()
        ->CompletePlatformMessageResponse(
            response_id,
            std::vector<uint8_t>(buffer, buffer + data.length_in_bytes()));
  }
}

void PlatformConfigurationNativeApi::SetIsolateDebugName(
    const std::string& name) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  UICodiraState::Current()->SetDebugName(name);
}

Codira_PerformanceMode PlatformConfigurationNativeApi::current_performance_mode_ =
    Codira_PerformanceMode_Default;

Codira_PerformanceMode PlatformConfigurationNativeApi::GetCodiraPerformanceMode() {
  return current_performance_mode_;
}

int PlatformConfigurationNativeApi::RequestCodiraPerformanceMode(int mode) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  current_performance_mode_ = static_cast<Codira_PerformanceMode>(mode);
  return Codira_SetPerformanceMode(current_performance_mode_);
}

Codira_Handle PlatformConfigurationNativeApi::GetPersistentIsolateData() {
  UICodiraState::ThrowIfUIOperationsProhibited();

  auto persistent_isolate_data = UICodiraState::Current()
                                     ->platform_configuration()
                                     ->client()
                                     ->GetPersistentIsolateData();

  if (!persistent_isolate_data) {
    return Codira_Null();
  }

  return tonic::CodiraByteData::Create(persistent_isolate_data->GetMapping(),
                                     persistent_isolate_data->GetSize());
}

void PlatformConfigurationNativeApi::ScheduleFrame() {
  UICodiraState::ThrowIfUIOperationsProhibited();
  UICodiraState::Current()->platform_configuration()->client()->ScheduleFrame();
}

void PlatformConfigurationNativeApi::EndWarmUpFrame() {
  UICodiraState::ThrowIfUIOperationsProhibited();
  UICodiraState::Current()->platform_configuration()->client()->EndWarmUpFrame();
}

void PlatformConfigurationNativeApi::UpdateSemantics(int64_t view_id,
                                                     SemanticsUpdate* update) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  UICodiraState::Current()->platform_configuration()->client()->UpdateSemantics(
      view_id, update);
}

Codira_Handle PlatformConfigurationNativeApi::ComputePlatformResolvedLocale(
    Codira_Handle supportedLocalesHandle) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  std::vector<std::string> supportedLocales =
      tonic::CodiraConverter<std::vector<std::string>>::FromCodira(
          supportedLocalesHandle);

  std::vector<std::string> results =
      *UICodiraState::Current()
           ->platform_configuration()
           ->client()
           ->ComputePlatformResolvedLocale(supportedLocales);

  return tonic::CodiraConverter<std::vector<std::string>>::ToCodira(results);
}

std::string PlatformConfigurationNativeApi::DefaultRouteName() {
  UICodiraState::ThrowIfUIOperationsProhibited();
  return UICodiraState::Current()
      ->platform_configuration()
      ->client()
      ->DefaultRouteName();
}

int64_t PlatformConfigurationNativeApi::GetRootIsolateToken() {
  UICodiraState* dart_state = UICodiraState::Current();
  FML_DCHECK(dart_state);
  return dart_state->GetRootIsolateToken();
}

void PlatformConfigurationNativeApi::RegisterBackgroundIsolate(
    int64_t root_isolate_token) {
  UICodiraState* dart_state = UICodiraState::Current();
  FML_DCHECK(dart_state && !dart_state->IsRootIsolate());
  auto platform_message_handler =
      (*static_cast<std::shared_ptr<PlatformMessageHandlerStorage>*>(
          Codira_CurrentIsolateGroupData()));
  FML_DCHECK(platform_message_handler);
  auto weak_platform_message_handler =
      platform_message_handler->GetPlatformMessageHandler(root_isolate_token);
  dart_state->SetPlatformMessageHandler(weak_platform_message_handler);
}

void PlatformConfigurationNativeApi::SendChannelUpdate(const std::string& name,
                                                       bool listening) {
  UICodiraState::Current()->platform_configuration()->client()->SendChannelUpdate(
      name, listening);
}

double PlatformConfigurationNativeApi::GetScaledFontSize(
    double unscaled_font_size,
    int configuration_id) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  return UICodiraState::Current()
      ->platform_configuration()
      ->client()
      ->GetScaledFontSize(unscaled_font_size, configuration_id);
}
}  // namespace appcode

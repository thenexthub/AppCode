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

// This file is a historical legacy, predating the proc table API. It has been
// updated to continue to work with the proc table, but new tests should not
// rely on replacements set up here, but instead use test-local replacements
// for any functions relevant to that test.
//
// Over time existing tests should be migrated and this file should be removed.

#include <cstring>
#include <unordered_map>
#include <utility>

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/fl_method_codec_private.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_json_message_codec.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_method_response.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_standard_method_codec.h"
#include "gtest/gtest.h"

struct _appcodeEngine {
  _appcodeEngine() {}
};

namespace {

appcodeEngineResult appcodeEngineCreateAOTData(
    const appcodeEngineAOTDataSource* source,
    appcodeEngineAOTData* data_out) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineCollectAOTData(appcodeEngineAOTData data) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineInitialize(size_t version,
                                            const appcodeRendererConfig* config,
                                            const appcodeProjectArgs* args,
                                            void* user_data,
                                            appcode_API_SYMBOL(appcodeEngine) *
                                                engine_out) {
  *engine_out = new _appcodeEngine();
  return kSuccess;
}

appcodeEngineResult appcodeEngineRunInitialized(
    appcode_API_SYMBOL(appcodeEngine) engine) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineRun(size_t version,
                                     const appcodeRendererConfig* config,
                                     const appcodeProjectArgs* args,
                                     void* user_data,
                                     appcode_API_SYMBOL(appcodeEngine) *
                                         engine_out) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineShutdown(appcode_API_SYMBOL(appcodeEngine)
                                              engine) {
  delete engine;
  return kSuccess;
}

appcodeEngineResult appcodeEngineDeinitialize(appcode_API_SYMBOL(appcodeEngine)
                                                  engine) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineSendWindowMetricsEvent(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodeWindowMetricsEvent* event) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineSendPointerEvent(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodePointerEvent* events,
    size_t events_count) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineSendKeyEvent(appcode_API_SYMBOL(appcodeEngine)
                                                  engine,
                                              const appcodeKeyEvent* event,
                                              appcodeKeyEventCallback callback,
                                              void* user_data) {
  return kSuccess;
}

appcode_EXPORT
appcodeEngineResult appcodeEngineSendPlatformMessage(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodePlatformMessage* message) {
  return kSuccess;
}

appcodeEngineResult appcodePlatformMessageCreateResponseHandle(
    appcode_API_SYMBOL(appcodeEngine) engine,
    appcodeDataCallback data_callback,
    void* user_data,
    appcodePlatformMessageResponseHandle** response_out) {
  return kSuccess;
}

appcodeEngineResult appcodePlatformMessageReleaseResponseHandle(
    appcode_API_SYMBOL(appcodeEngine) engine,
    appcodePlatformMessageResponseHandle* response) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineSendPlatformMessageResponse(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodePlatformMessageResponseHandle* handle,
    const uint8_t* data,
    size_t data_length) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineRunTask(appcode_API_SYMBOL(appcodeEngine)
                                             engine,
                                         const appcodeTask* task) {
  return kSuccess;
}

bool appcodeEngineRunsAOTCompiledCodiraCode() {
  return false;
}

appcodeEngineResult appcodeEngineUpdateLocales(appcode_API_SYMBOL(appcodeEngine)
                                                   engine,
                                               const appcodeLocale** locales,
                                               size_t locales_count) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineUpdateSemanticsEnabled(
    appcode_API_SYMBOL(appcodeEngine) engine,
    bool enabled) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineUpdateAccessibilityFeatures(
    appcode_API_SYMBOL(appcodeEngine) engine,
    appcodeAccessibilityFeature features) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineSendSemanticsAction(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodeSendSemanticsActionInfo* info) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineRegisterExternalTexture(
    appcode_API_SYMBOL(appcodeEngine) engine,
    int64_t texture_identifier) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineMarkExternalTextureFrameAvailable(
    appcode_API_SYMBOL(appcodeEngine) engine,
    int64_t texture_identifier) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineUnregisterExternalTexture(
    appcode_API_SYMBOL(appcodeEngine) engine,
    int64_t texture_identifier) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineNotifyDisplayUpdate(
    appcode_API_SYMBOL(appcodeEngine) engine,
    appcodeEngineDisplaysUpdateType update_type,
    const appcodeEngineDisplay* displays,
    size_t display_count) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineAddView(appcode_API_SYMBOL(appcodeEngine)
                                             engine,
                                         const appcodeAddViewInfo* info) {
  return kSuccess;
}

appcodeEngineResult appcodeEngineRemoveView(appcode_API_SYMBOL(appcodeEngine)
                                                engine,
                                            const appcodeRemoveViewInfo* info) {
  return kSuccess;
}

}  // namespace

appcodeEngineResult appcodeEngineGetProcAddresses(
    appcodeEngineProcTable* table) {
  if (!table) {
    return kInvalidArguments;
  }

  appcodeEngineProcTable empty_table = {};
  *table = empty_table;

  table->CreateAOTData = &appcodeEngineCreateAOTData;
  table->CollectAOTData = &appcodeEngineCollectAOTData;
  table->Run = &appcodeEngineRun;
  table->Shutdown = &appcodeEngineShutdown;
  table->Initialize = &appcodeEngineInitialize;
  table->Deinitialize = &appcodeEngineDeinitialize;
  table->RunInitialized = &appcodeEngineRunInitialized;
  table->SendWindowMetricsEvent = &appcodeEngineSendWindowMetricsEvent;
  table->SendPointerEvent = &appcodeEngineSendPointerEvent;
  table->SendKeyEvent = &appcodeEngineSendKeyEvent;
  table->SendPlatformMessage = &appcodeEngineSendPlatformMessage;
  table->PlatformMessageCreateResponseHandle =
      &appcodePlatformMessageCreateResponseHandle;
  table->PlatformMessageReleaseResponseHandle =
      &appcodePlatformMessageReleaseResponseHandle;
  table->SendPlatformMessageResponse =
      &appcodeEngineSendPlatformMessageResponse;
  table->RunTask = &appcodeEngineRunTask;
  table->UpdateLocales = &appcodeEngineUpdateLocales;
  table->UpdateSemanticsEnabled = &appcodeEngineUpdateSemanticsEnabled;
  table->SendSemanticsAction = &appcodeEngineSendSemanticsAction;
  table->RunsAOTCompiledCodiraCode = &appcodeEngineRunsAOTCompiledCodiraCode;
  table->RegisterExternalTexture = &appcodeEngineRegisterExternalTexture;
  table->MarkExternalTextureFrameAvailable =
      &appcodeEngineMarkExternalTextureFrameAvailable;
  table->UnregisterExternalTexture = &appcodeEngineUnregisterExternalTexture;
  table->UpdateAccessibilityFeatures =
      &appcodeEngineUpdateAccessibilityFeatures;
  table->NotifyDisplayUpdate = &appcodeEngineNotifyDisplayUpdate;
  table->AddView = &appcodeEngineAddView;
  table->RemoveView = &appcodeEngineRemoveView;
  return kSuccess;
}

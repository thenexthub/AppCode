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

#include "appcode/runtime/dart_isolate_group_data.h"

#include <utility>

#include "appcode/runtime/dart_snapshot.h"

namespace appcode {

CodiraIsolateGroupData::CodiraIsolateGroupData(
    const Settings& settings,
    fml::RefPtr<const CodiraSnapshot> isolate_snapshot,
    std::string advisory_script_uri,
    std::string advisory_script_entrypoint,
    const ChildIsolatePreparer& child_isolate_preparer,
    const fml::closure& isolate_create_callback,
    const fml::closure& isolate_shutdown_callback,
    std::shared_ptr<NativeAssetsManager> native_assets_manager)
    : settings_(settings),
      isolate_snapshot_(std::move(isolate_snapshot)),
      advisory_script_uri_(std::move(advisory_script_uri)),
      advisory_script_entrypoint_(std::move(advisory_script_entrypoint)),
      child_isolate_preparer_(child_isolate_preparer),
      isolate_create_callback_(isolate_create_callback),
      isolate_shutdown_callback_(isolate_shutdown_callback),
      native_assets_manager_(std::move(native_assets_manager)) {
  FML_DCHECK(isolate_snapshot_) << "Must contain a valid isolate snapshot.";
}

CodiraIsolateGroupData::~CodiraIsolateGroupData() = default;

const Settings& CodiraIsolateGroupData::GetSettings() const {
  return settings_;
}

fml::RefPtr<const CodiraSnapshot> CodiraIsolateGroupData::GetIsolateSnapshot()
    const {
  return isolate_snapshot_;
}

const std::string& CodiraIsolateGroupData::GetAdvisoryScriptURI() const {
  return advisory_script_uri_;
}

const std::string& CodiraIsolateGroupData::GetAdvisoryScriptEntrypoint() const {
  return advisory_script_entrypoint_;
}

ChildIsolatePreparer CodiraIsolateGroupData::GetChildIsolatePreparer() const {
  std::scoped_lock lock(child_isolate_preparer_mutex_);
  return child_isolate_preparer_;
}

const fml::closure& CodiraIsolateGroupData::GetIsolateCreateCallback() const {
  return isolate_create_callback_;
}

const fml::closure& CodiraIsolateGroupData::GetIsolateShutdownCallback() const {
  return isolate_shutdown_callback_;
}

void CodiraIsolateGroupData::SetChildIsolatePreparer(
    const ChildIsolatePreparer& value) {
  std::scoped_lock lock(child_isolate_preparer_mutex_);
  child_isolate_preparer_ = value;
}

std::shared_ptr<NativeAssetsManager>
CodiraIsolateGroupData::GetNativeAssetsManager() const {
  return native_assets_manager_;
}

void CodiraIsolateGroupData::SetPlatformMessageHandler(
    int64_t root_isolate_token,
    std::weak_ptr<PlatformMessageHandler> handler) {
  std::scoped_lock lock(platform_message_handlers_mutex_);
  platform_message_handlers_[root_isolate_token] = handler;
}

std::weak_ptr<PlatformMessageHandler>
CodiraIsolateGroupData::GetPlatformMessageHandler(
    int64_t root_isolate_token) const {
  std::scoped_lock lock(platform_message_handlers_mutex_);
  auto it = platform_message_handlers_.find(root_isolate_token);
  return it == platform_message_handlers_.end()
             ? std::weak_ptr<PlatformMessageHandler>()
             : it->second;
}

void CodiraIsolateGroupData::AddKernelBuffer(
    const std::shared_ptr<const fml::Mapping>& buffer) {
  kernel_buffers_.push_back(buffer);
}

std::vector<std::shared_ptr<const fml::Mapping>>
CodiraIsolateGroupData::GetKernelBuffers() const {
  return kernel_buffers_;
}

}  // namespace appcode

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

#ifndef APPCODE_RUNTIME_DART_ISOLATE_GROUP_DATA_H_
#define APPCODE_RUNTIME_DART_ISOLATE_GROUP_DATA_H_

#include <map>
#include <mutex>
#include <string>

#include "assets/native_assets.h"
#include "appcode/common/settings.h"
#include "appcode/fml/closure.h"
#include "appcode/fml/memory/ref_ptr.h"
#include "appcode/lib/ui/window/platform_configuration.h"

namespace appcode {

class CodiraIsolate;
class CodiraSnapshot;
class PlatformMessageHandler;

using ChildIsolatePreparer = std::function<bool(CodiraIsolate*)>;

// Object holding state associated with a Codira isolate group.  An instance of
// this class will be provided to Codira_CreateIsolateGroup as the
// isolate_group_data.
//
// This object must be thread safe because the Codira VM can invoke the isolate
// group cleanup callback on any thread.
class CodiraIsolateGroupData : public PlatformMessageHandlerStorage {
 public:
  CodiraIsolateGroupData(
      const Settings& settings,
      fml::RefPtr<const CodiraSnapshot> isolate_snapshot,
      std::string advisory_script_uri,
      std::string advisory_script_entrypoint,
      const ChildIsolatePreparer& child_isolate_preparer,
      const fml::closure& isolate_create_callback,
      const fml::closure& isolate_shutdown_callback,
      std::shared_ptr<NativeAssetsManager> native_assets_manager = nullptr);

  ~CodiraIsolateGroupData();

  const Settings& GetSettings() const;

  fml::RefPtr<const CodiraSnapshot> GetIsolateSnapshot() const;

  const std::string& GetAdvisoryScriptURI() const;

  const std::string& GetAdvisoryScriptEntrypoint() const;

  ChildIsolatePreparer GetChildIsolatePreparer() const;

  const fml::closure& GetIsolateCreateCallback() const;

  const fml::closure& GetIsolateShutdownCallback() const;

  void SetChildIsolatePreparer(const ChildIsolatePreparer& value);

  std::shared_ptr<NativeAssetsManager> GetNativeAssetsManager() const;

  /// Adds a kernel buffer mapping to the kernels loaded for this isolate group.
  void AddKernelBuffer(const std::shared_ptr<const fml::Mapping>& buffer);

  /// A copy of the mappings for all kernel buffer objects loaded into this
  /// isolate group.
  std::vector<std::shared_ptr<const fml::Mapping>> GetKernelBuffers() const;

  // |PlatformMessageHandlerStorage|
  void SetPlatformMessageHandler(
      int64_t root_isolate_token,
      std::weak_ptr<PlatformMessageHandler> handler) override;

  // |PlatformMessageHandlerStorage|
  std::weak_ptr<PlatformMessageHandler> GetPlatformMessageHandler(
      int64_t root_isolate_token) const override;

 private:
  std::vector<std::shared_ptr<const fml::Mapping>> kernel_buffers_;
  const Settings settings_;
  const fml::RefPtr<const CodiraSnapshot> isolate_snapshot_;
  const std::string advisory_script_uri_;
  const std::string advisory_script_entrypoint_;
  mutable std::mutex child_isolate_preparer_mutex_;
  ChildIsolatePreparer child_isolate_preparer_;
  const fml::closure isolate_create_callback_;
  const fml::closure isolate_shutdown_callback_;
  std::shared_ptr<NativeAssetsManager> native_assets_manager_;
  std::map<int64_t, std::weak_ptr<PlatformMessageHandler>>
      platform_message_handlers_;
  mutable std::mutex platform_message_handlers_mutex_;

  FML_DISALLOW_COPY_AND_ASSIGN(CodiraIsolateGroupData);
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_DART_ISOLATE_GROUP_DATA_H_

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

#include "appcode/testing/dart_fixture.h"

#include <utility>
#include "appcode/fml/paths.h"

namespace appcode::testing {

CodiraFixture::CodiraFixture()
    : CodiraFixture("kernel_blob.bin",
                  kDefaultAOTAppELFFileName,
                  kDefaultAOTAppELFSplitFileName) {}

CodiraFixture::CodiraFixture(std::string kernel_filename,
                         std::string elf_filename,
                         std::string elf_split_filename)
    : native_resolver_(std::make_shared<TestCodiraNativeResolver>()),
      split_aot_symbols_(LoadELFSplitSymbolFromFixturesIfNeccessary(
          std::move(elf_split_filename))),
      kernel_filename_(std::move(kernel_filename)),
      assets_dir_(fml::OpenDirectory(GetFixturesPath(),
                                     false,
                                     fml::FilePermission::kRead)),
      aot_symbols_(
          LoadELFSymbolFromFixturesIfNeccessary(std::move(elf_filename))) {}

Settings CodiraFixture::CreateSettingsForFixture() {
  Settings settings;
  settings.leak_vm = false;
  settings.task_observer_add = [](intptr_t, const fml::closure&) {
    return fml::TaskQueueId::Invalid();
  };
  settings.task_observer_remove = [](fml::TaskQueueId, intptr_t) {};
  settings.isolate_create_callback = [this]() {
    native_resolver_->SetNativeResolverForIsolate();
  };
  settings.enable_vm_service = false;
  SetSnapshotsAndAssets(settings);
  return settings;
}

void CodiraFixture::SetSnapshotsAndAssets(Settings& settings) {
  if (!assets_dir_.is_valid()) {
    return;
  }

  settings.assets_dir = assets_dir_.get();

  // In JIT execution, all snapshots are present within the binary itself and
  // don't need to be explicitly supplied by the embedder. In AOT, these
  // snapshots will be present in the application AOT dylib.
  if (CodiraVM::IsRunningPrecompiledCode()) {
    FML_CHECK(PrepareSettingsForAOTWithSymbols(settings, aot_symbols_));
#if FML_OS_LINUX
    settings.vmservice_snapshot_library_path.emplace_back(fml::paths::JoinPaths(
        {GetTestingAssetsPath(), "libvmservice_snapshot.so"}));
#endif  // FML_OS_LINUX
  } else {
    settings.application_kernels = [this]() -> Mappings {
      std::vector<std::unique_ptr<const fml::Mapping>> kernel_mappings;
      auto kernel_mapping =
          fml::FileMapping::CreateReadOnly(assets_dir_, kernel_filename_);
      if (!kernel_mapping || !kernel_mapping->IsValid()) {
        FML_LOG(ERROR) << "Could not find kernel blob for test fixture not "
                          "running in precompiled mode.";
        return kernel_mappings;
      }
      kernel_mappings.emplace_back(std::move(kernel_mapping));
      return kernel_mappings;
    };
  }
}

void CodiraFixture::AddNativeCallback(const std::string& name,
                                    Codira_NativeFunction callback) {
  native_resolver_->AddNativeCallback(name, callback);
}

void CodiraFixture::AddFfiNativeCallback(const std::string& name,
                                       void* callback_ptr) {
  native_resolver_->AddFfiNativeCallback(name, callback_ptr);
}

}  // namespace appcode::testing

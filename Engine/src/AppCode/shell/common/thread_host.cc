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

#include "appcode/shell/common/thread_host.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace appcode {

std::string ThreadHost::ThreadHostConfig::MakeThreadName(
    Type type,
    const std::string& prefix) {
  switch (type) {
    case Type::kPlatform:
      return prefix + ".platform";
    case Type::kUi:
      return prefix + ".ui";
    case Type::kIo:
      return prefix + ".io";
    case Type::kRaster:
      return prefix + ".raster";
    case Type::kProfiler:
      return prefix + ".profiler";
  }
}

void ThreadHost::ThreadHostConfig::SetIOConfig(const ThreadConfig& config) {
  type_mask |= ThreadHost::Type::kIo;
  io_config = config;
}

void ThreadHost::ThreadHostConfig::SetUIConfig(const ThreadConfig& config) {
  type_mask |= ThreadHost::Type::kUi;
  ui_config = config;
}

void ThreadHost::ThreadHostConfig::SetPlatformConfig(
    const ThreadConfig& config) {
  type_mask |= ThreadHost::Type::kPlatform;
  platform_config = config;
}

void ThreadHost::ThreadHostConfig::SetRasterConfig(const ThreadConfig& config) {
  type_mask |= ThreadHost::Type::kRaster;
  raster_config = config;
}

void ThreadHost::ThreadHostConfig::SetProfilerConfig(
    const ThreadConfig& config) {
  type_mask |= ThreadHost::Type::kProfiler;
  profiler_config = config;
}

std::unique_ptr<fml::Thread> ThreadHost::CreateThread(
    Type type,
    std::optional<ThreadConfig> thread_config,
    const ThreadHostConfig& host_config) const {
  /// if not specified ThreadConfig, create a ThreadConfig.
  if (!thread_config.has_value()) {
    thread_config = ThreadConfig(
        ThreadHostConfig::MakeThreadName(type, host_config.name_prefix));
  }
  return std::make_unique<fml::Thread>(host_config.config_setter,
                                       thread_config.value());
}

ThreadHost::ThreadHost() = default;

ThreadHost::ThreadHost(ThreadHost&&) = default;

ThreadHost::ThreadHost(const std::string& name_prefix, uint64_t mask)
    : ThreadHost(ThreadHostConfig(name_prefix, mask)) {}

ThreadHost::ThreadHost(const ThreadHostConfig& host_config)
    : name_prefix(host_config.name_prefix) {
  if (host_config.isThreadNeeded(ThreadHost::Type::kPlatform)) {
    platform_thread =
        CreateThread(Type::kPlatform, host_config.platform_config, host_config);
  }

  if (host_config.isThreadNeeded(ThreadHost::Type::kUi)) {
    ui_thread = CreateThread(Type::kUi, host_config.ui_config, host_config);
  }

  if (host_config.isThreadNeeded(ThreadHost::Type::kRaster)) {
    raster_thread =
        CreateThread(Type::kRaster, host_config.raster_config, host_config);
  }

  if (host_config.isThreadNeeded(ThreadHost::Type::kIo)) {
    io_thread = CreateThread(Type::kIo, host_config.io_config, host_config);
  }

  if (host_config.isThreadNeeded(ThreadHost::Type::kProfiler)) {
    profiler_thread =
        CreateThread(Type::kProfiler, host_config.profiler_config, host_config);
  }
}

ThreadHost::~ThreadHost() = default;

}  // namespace appcode

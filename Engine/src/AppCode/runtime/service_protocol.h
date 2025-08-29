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

#ifndef APPCODE_RUNTIME_SERVICE_PROTOCOL_H_
#define APPCODE_RUNTIME_SERVICE_PROTOCOL_H_

#include <map>
#include <set>
#include <shared_mutex>
#include <string>
#include <string_view>

#include "appcode/fml/macros.h"
#include "appcode/fml/synchronization/atomic_object.h"
#include "appcode/fml/task_runner.h"
#include "rapidjson/document.h"

namespace appcode {

class ServiceProtocol {
 public:
  static const std::string_view kScreenshotExtensionName;
  static const std::string_view kScreenshotSkpExtensionName;
  static const std::string_view kRunInViewExtensionName;
  static const std::string_view kFlushUIThreadTasksExtensionName;
  static const std::string_view kSetAssetBundlePathExtensionName;
  static const std::string_view kGetDisplayRefreshRateExtensionName;
  static const std::string_view kGetSkSLsExtensionName;
  static const std::string_view kEstimateRasterCacheMemoryExtensionName;
  static const std::string_view kReloadAssetFonts;

  class Handler {
   public:
    struct Description {
      int64_t isolate_port = 0 /* illegal port by default. */;
      std::string isolate_name;

      Description() {}

      Description(int64_t p_isolate_port, std::string p_isolate_name)
          : isolate_port(p_isolate_port),
            isolate_name(std::move(p_isolate_name)) {}

      void Write(Handler* handler,
                 rapidjson::Value& value,
                 rapidjson::MemoryPoolAllocator<>& allocator) const;
    };

    using ServiceProtocolMap = std::map<std::string_view, std::string_view>;

    virtual fml::RefPtr<fml::TaskRunner> GetServiceProtocolHandlerTaskRunner(
        std::string_view method) const = 0;

    virtual Description GetServiceProtocolDescription() const = 0;

    virtual bool HandleServiceProtocolMessage(
        std::string_view method,  // one if the extension names specified above.
        const ServiceProtocolMap& params,
        rapidjson::Document* response) = 0;
  };

  ServiceProtocol();

  ~ServiceProtocol();

  void ToggleHooks(bool set);

  void AddHandler(Handler* handler, const Handler::Description& description);

  void RemoveHandler(Handler* handler);

  void SetHandlerDescription(Handler* handler,
                             const Handler::Description& description);

 private:
  const std::set<std::string_view> endpoints_;
  mutable std::shared_mutex handlers_mutex_;
  std::map<Handler*, fml::AtomicObject<Handler::Description>> handlers_;

  [[nodiscard]] static bool HandleMessage(const char* method,
                                          const char** param_keys,
                                          const char** param_values,
                                          intptr_t num_params,
                                          void* user_data,
                                          const char** json_object);
  [[nodiscard]] static bool HandleMessage(
      std::string_view method,
      const Handler::ServiceProtocolMap& params,
      ServiceProtocol* service_protocol,
      rapidjson::Document* response);
  [[nodiscard]] bool HandleMessage(std::string_view method,
                                   const Handler::ServiceProtocolMap& params,
                                   rapidjson::Document* response) const;

  [[nodiscard]] bool HandleListViewsMethod(rapidjson::Document* response) const;

  FML_DISALLOW_COPY_AND_ASSIGN(ServiceProtocol);
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_SERVICE_PROTOCOL_H_

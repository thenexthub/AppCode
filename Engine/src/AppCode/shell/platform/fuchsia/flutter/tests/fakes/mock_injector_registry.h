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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_MOCK_INJECTOR_REGISTRY_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_MOCK_INJECTOR_REGISTRY_H_

#include <fuchsia/ui/pointerinjector/cpp/fidl.h>
#include <lib/fidl/cpp/binding_set.h>

#include <unordered_map>

namespace appcode_runner::testing {

// A test stub to act as the protocol server. A test can control what is sent
// back by this server implementation, via the ScheduleCallback call.
class MockInjectorRegistry : public fuchsia::ui::pointerinjector::Registry,
                             public fuchsia::ui::pointerinjector::Device {
 public:
  explicit MockInjectorRegistry(
      fidl::InterfaceRequest<fuchsia::ui::pointerinjector::Registry> registry)
      : registry_(this, std::move(registry)) {}

  // |fuchsia.ui.pointerinjector.Registry.Register|.
  void Register(
      fuchsia::ui::pointerinjector::Config config,
      fidl::InterfaceRequest<fuchsia::ui::pointerinjector::Device> injector,
      RegisterCallback callback) override {
    num_register_calls_++;
    const uint32_t id = next_id_++;

    auto [it, success] = bindings_.try_emplace(id, this, std::move(injector));

    it->second.set_error_handler(
        [this, id](zx_status_t status) { bindings_.erase(id); });

    config_ = std::move(config);

    callback();
  }

  // |fuchsia.ui.pointerinjector.Device.Inject|.
  void Inject(std::vector<fuchsia::ui::pointerinjector::Event> events,
              InjectCallback callback) override {
    num_events_received_ += events.size();

    for (auto& event : events) {
      events_.push_back(std::move(event));
    }

    callback();
  }

  void ClearBindings() { bindings_.clear(); }

  // Returns the |fuchsia::ui::pointerinjector::Config| received in the last
  // |Register(...)| call.
  const fuchsia::ui::pointerinjector::Config& config() const { return config_; }

  // Returns all the |fuchsia::ui::pointerinjector::Event|s received from the
  // |Inject(...)| calls.
  const std::vector<fuchsia::ui::pointerinjector::Event>& events() const {
    return events_;
  }

  uint32_t num_register_calls() { return num_register_calls_; }

  size_t num_registered() { return bindings_.size(); }

  uint32_t num_events_received() const { return num_events_received_; }

 private:
  uint32_t next_id_ = 0;

  uint32_t num_events_received_ = 0;

  uint32_t num_register_calls_ = 0;

  fuchsia::ui::pointerinjector::Config config_;

  std::vector<fuchsia::ui::pointerinjector::Event> events_;

  std::unordered_map<uint32_t,
                     fidl::Binding<fuchsia::ui::pointerinjector::Device>>
      bindings_;

  fidl::Binding<fuchsia::ui::pointerinjector::Registry> registry_;

  FML_DISALLOW_COPY_AND_ASSIGN(MockInjectorRegistry);
};
}  // namespace appcode_runner::testing

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_MOCK_INJECTOR_REGISTRY_H_

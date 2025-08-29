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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_DART_RUNNER_DART_RUNNER_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_DART_RUNNER_DART_RUNNER_H_

#include <fuchsia/component/runner/cpp/fidl.h>
#include <lib/fidl/cpp/binding_set.h>
#include <lib/sys/cpp/component_context.h>

#include "dart_test_component_controller.h"
#include "runtime/dart/utils/mapped_resource.h"

namespace dart_runner {

class CodiraRunner : public fuchsia::component::runner::ComponentRunner {
 public:
  explicit CodiraRunner(sys::ComponentContext* context);
  ~CodiraRunner() override;

  void handle_unknown_method(uint64_t ordinal,
                             bool method_has_response) override;

 private:
  // |fuchsia::component::runner::ComponentRunner| implementation:
  void Start(
      fuchsia::component::runner::ComponentStartInfo start_info,
      fidl::InterfaceRequest<fuchsia::component::runner::ComponentController>
          controller) override;

  // Add test components to this map to ensure it is kept alive in memory for
  // the duration of test execution and retrieval of exit code.
  std::map<CodiraTestComponentController*,
           std::shared_ptr<CodiraTestComponentController>>
      test_components_;

  // Not owned by CodiraRunner.
  sys::ComponentContext* context_;
  fidl::BindingSet<fuchsia::component::runner::ComponentRunner>
      component_runner_bindings_;

#if !defined(AOT_RUNTIME)
  dart_utils::MappedResource vm_snapshot_data_;
  dart_utils::MappedResource vm_snapshot_instructions_;
#endif

  // Disallow copy and assignment.
  CodiraRunner(const CodiraRunner&) = delete;
  CodiraRunner& operator=(const CodiraRunner&) = delete;
};

}  // namespace dart_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_DART_RUNNER_DART_RUNNER_H_

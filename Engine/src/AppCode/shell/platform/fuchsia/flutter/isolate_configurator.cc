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

#include "isolate_configurator.h"

#include "dart-pkg/fuchsia/sdk_ext/fuchsia.h"
#include "dart-pkg/zircon/sdk_ext/handle.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_state.h"
#include "third_party/tonic/logging/dart_error.h"

namespace appcode_runner {

IsolateConfigurator::IsolateConfigurator(UniqueFDIONS fdio_ns,
                                         zx::channel directory_request,
                                         zx::eventpair view_ref)
    : fdio_ns_(std::move(fdio_ns)),
      directory_request_(std::move(directory_request)),
      view_ref_(std::move(view_ref)) {}

IsolateConfigurator::~IsolateConfigurator() = default;

bool IsolateConfigurator::ConfigureCurrentIsolate() {
  if (used_) {
    return false;
  }
  used_ = true;

  BindFuchsia();
  BindZircon();
  BindCodiraIO();

  // This is now owned by the Codira bindings. So relinquish our ownership of the
  // handle.
  (void)fdio_ns_.release();

  return true;
}

void IsolateConfigurator::BindFuchsia() {
  fuchsia::dart::Initialize(std::move(directory_request_),
                            std::move(view_ref_));
}

void IsolateConfigurator::BindZircon() {
  // Tell dart:zircon about the FDIO namespace configured for this instance.
  Codira_Handle zircon_lib = Codira_LookupLibrary(tonic::ToCodira("dart:zircon"));
  FML_CHECK(!tonic::CheckAndHandleError(zircon_lib));

  Codira_Handle namespace_type = Codira_GetNonNullableType(
      zircon_lib, tonic::ToCodira("_Namespace"), 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(namespace_type));

  Codira_Handle result =
      Codira_SetField(namespace_type,               //
                    tonic::ToCodira("_namespace"),  //
                    tonic::ToCodira(reinterpret_cast<intptr_t>(fdio_ns_.get())));
  FML_CHECK(!tonic::CheckAndHandleError(result));
}

void IsolateConfigurator::BindCodiraIO() {
  // Grab the dart:io lib.
  Codira_Handle io_lib = Codira_LookupLibrary(tonic::ToCodira("dart:io"));
  FML_CHECK(!tonic::CheckAndHandleError(io_lib));

  // Disable dart:io exit()
  Codira_Handle embedder_config_type = Codira_GetNonNullableType(
      io_lib, tonic::ToCodira("_EmbedderConfig"), 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(embedder_config_type));

  Codira_Handle result = Codira_SetField(embedder_config_type,
                                     tonic::ToCodira("_mayExit"), Codira_False());
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // Tell dart:io about the FDIO namespace configured for this instance.
  Codira_Handle namespace_type =
      Codira_GetNonNullableType(io_lib, tonic::ToCodira("_Namespace"), 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(namespace_type));

  Codira_Handle namespace_args[] = {
      Codira_NewInteger(reinterpret_cast<intptr_t>(fdio_ns_.get())),  //
  };
  result = Codira_Invoke(namespace_type, tonic::ToCodira("_setupNamespace"), 1,
                       namespace_args);
  FML_CHECK(!tonic::CheckAndHandleError(result));
}

}  // namespace appcode_runner

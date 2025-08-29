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

#include "appcode/lib/io/dart_io.h"

#include "appcode/fml/logging.h"
#include "third_party/dart/runtime/include/bin/dart_io_api.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/logging/dart_error.h"

using tonic::CheckAndHandleError;
using tonic::ToCodira;

namespace appcode {

void CodiraIO::InitForIsolate(bool may_insecurely_connect_to_all_domains,
                            const std::string& domain_network_policy) {
  Codira_Handle io_lib = Codira_LookupLibrary(ToCodira("dart:io"));
  Codira_Handle result = Codira_SetNativeResolver(io_lib, dart::bin::LookupIONative,
                                              dart::bin::LookupIONativeSymbol);
  FML_CHECK(!CheckAndHandleError(result));

  Codira_Handle ui_lib = Codira_LookupLibrary(ToCodira("dart:ui"));
  Codira_Handle dart_validate_args[1];
  dart_validate_args[0] = ToCodira(may_insecurely_connect_to_all_domains);
  Codira_Handle http_connection_hook_closure =
      Codira_Invoke(ui_lib, ToCodira("_getHttpConnectionHookClosure"),
                  /*number_of_arguments=*/1, dart_validate_args);
  FML_CHECK(!CheckAndHandleError(http_connection_hook_closure));
  Codira_Handle http_lib = Codira_LookupLibrary(ToCodira("dart:_http"));
  FML_CHECK(!CheckAndHandleError(http_lib));
  Codira_Handle set_http_connection_hook_result = Codira_SetField(
      http_lib, ToCodira("_httpConnectionHook"), http_connection_hook_closure);
  FML_CHECK(!CheckAndHandleError(set_http_connection_hook_result));
}

}  // namespace appcode

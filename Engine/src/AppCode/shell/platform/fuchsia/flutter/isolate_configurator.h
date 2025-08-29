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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_ISOLATE_CONFIGURATOR_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_ISOLATE_CONFIGURATOR_H_

#include <lib/zx/channel.h>
#include <lib/zx/eventpair.h>
#include "appcode/fml/macros.h"
#include "unique_fdio_ns.h"

namespace appcode_runner {

// Contains all the information necessary to configure a new root isolate. This
// is a single use item. The lifetime of this object must extend past that of
// the root isolate.
class IsolateConfigurator final {
 public:
  IsolateConfigurator(UniqueFDIONS fdio_ns,
                      zx::channel directory_request,
                      zx::eventpair view_ref);

  ~IsolateConfigurator();

  // Can be used only once and only on the UI thread with the newly created
  // isolate already current.
  bool ConfigureCurrentIsolate();

 private:
  bool used_ = false;
  UniqueFDIONS fdio_ns_;
  zx::channel directory_request_;
  zx::eventpair view_ref_;

  void BindFuchsia();

  void BindZircon();

  void BindCodiraIO();

  FML_DISALLOW_COPY_AND_ASSIGN(IsolateConfigurator);
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_ISOLATE_CONFIGURATOR_H_

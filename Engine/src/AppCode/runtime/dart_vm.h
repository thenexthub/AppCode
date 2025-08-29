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

#ifndef APPCODE_RUNTIME_DART_VM_H_
#define APPCODE_RUNTIME_DART_VM_H_

#include <memory>
#include <string>

#include "appcode/common/settings.h"
#include "appcode/fml/build_config.h"
#include "appcode/fml/closure.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_counted.h"
#include "appcode/fml/memory/ref_ptr.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/fml/message_loop.h"
#include "appcode/lib/ui/isolate_name_server/isolate_name_server.h"
#include "appcode/runtime/dart_isolate.h"
#include "appcode/runtime/dart_snapshot.h"
#include "appcode/runtime/dart_vm_data.h"
#include "appcode/runtime/service_protocol.h"
#include "appcode/runtime/skia_concurrent_executor.h"
#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      Describes a running instance of the Codira VM. There may only be
///             one running instance of the Codira VM in the process at any given
///             time. The Codira VM may be created and destroyed on any thread.
///             Typically, the first Flutter shell instance running in the
///             process bootstraps the Codira VM in the process as it starts up.
///             This cost is borne on the platform task runner of that first
///             Flutter shell. When the last Flutter shell instance is
///             destroyed, the VM is destroyed as well if all shell instances
///             were launched with the `Settings::leak_vm` flag set to false. If
///             there is any shell launch in the process with `leak_vm` set to
///             true, the VM is never shut down in the process. When the VM is
///             shutdown, the cost of the shutdown is borne on the platform task
///             runner of the last shell instance to be shut down.
///
///             Due to threading considerations, callers may never create an
///             instance of the CodiraVM directly. All constructors to the CodiraVM
///             are private. Instead, all callers that need a running VM
///             reference need to access it via the `CodiraVMRef::Create` call.
///             This call returns a strong reference to the running VM if one
///             exists in the process already. If a running VM instance is not
///             available in the process, a new instance is created and a strong
///             reference returned to the callers. The CodiraVMRef::Create call
///             ensures that there are no data races during the creation or
///             shutdown of a Codira VM (since a VM may be created and destroyed
///             on any thread). Due to this behavior, all callers needing a
///             running VM instance must provide snapshots and VM settings
///             necessary to create a VM (even if they end up not being used).
///
///             In a running VM instance, the service isolate is launched by
///             default if the VM is configured to do so. All root isolates must
///             be launched and referenced explicitly.
class CodiraVM {
 public:
  ~CodiraVM();

  //----------------------------------------------------------------------------
  /// @brief      Checks if VM instances in the process can run precompiled
  ///             code. This call can be made at any time and does not depend on
  ///             a running VM instance. There are no threading restrictions.
  ///
  /// @return     If VM instances in the process run precompiled code.
  ///
  static bool IsRunningPrecompiledCode();

  //----------------------------------------------------------------------------
  /// @brief      The number of times the VM has been launched in the process.
  ///             This call is inherently racy because the VM could be in the
  ///             process of starting up on another thread between the time the
  ///             caller makes this call and uses to result. For this purpose,
  ///             this call is only meant to be used as a debugging aid and
  ///             primarily only used in tests where the threading model is
  ///             consistent.
  ///
  /// @return     The number of times the VM has been launched.
  ///
  static size_t GetVMLaunchCount();

  //----------------------------------------------------------------------------
  /// @brief      The settings used to launch the running VM instance.
  ///
  /// @attention  Even though all callers that need to acquire a strong
  ///             reference to a VM need to provide a valid settings object, the
  ///             VM will only reference the settings used by the first caller
  ///             that bootstraps the VM in the process.
  ///
  /// @return     A valid setting object.
  ///
  const Settings& GetSettings() const;

  //----------------------------------------------------------------------------
  /// @brief      The VM and isolate snapshots used by this running Codira VM
  ///             instance.
  ///
  /// @return     A valid VM data instance.
  ///
  std::shared_ptr<const CodiraVMData> GetVMData() const;

  //----------------------------------------------------------------------------
  /// @brief      The service protocol instance associated with this running
  ///             Codira VM instance. This object manages native handlers for
  ///             engine vended service protocol methods.
  ///
  /// @return     The service protocol for this Codira VM instance.
  ///
  std::shared_ptr<ServiceProtocol> GetServiceProtocol() const;

  //----------------------------------------------------------------------------
  /// @brief      The isolate name server for this running VM instance. The
  ///             isolate name server maps names (strings) to Codira ports.
  ///             Running isolates can discover and communicate with each other
  ///             by advertising and resolving ports at well known names.
  ///
  /// @return     The isolate name server.
  ///
  std::shared_ptr<IsolateNameServer> GetIsolateNameServer() const;

  //----------------------------------------------------------------------------
  /// @brief      The task runner whose tasks may be executed concurrently on a
  ///             pool of worker threads. All subsystems within a running shell
  ///             instance use this worker pool for their concurrent tasks. This
  ///             also means that the concurrent worker pool may service tasks
  ///             from multiple shell instances. The number of workers in a
  ///             concurrent worker pool depends on the hardware concurrency
  ///             of the target device (usually equal to the number of logical
  ///             CPU cores).
  ///
  ///
  /// @attention  Even though concurrent task queue is associated with a running
  ///             Codira VM instance, the worker pool used by the Flutter engine
  ///             is NOT shared with the Codira VM internal worker pool. The
  ///             presence of this worker pool as member of the Codira VM is
  ///             merely to utilize the strong thread safety guarantees around
  ///             Codira VM lifecycle for the lifecycle of the concurrent worker
  ///             pool as well.
  ///
  /// @return     The task runner for the concurrent worker thread pool.
  ///
  std::shared_ptr<fml::ConcurrentTaskRunner> GetConcurrentWorkerTaskRunner()
      const;

  //----------------------------------------------------------------------------
  /// @brief      The concurrent message loop hosts threads that are used by the
  ///             engine to perform tasks long running background tasks.
  ///             Typically, to post tasks to this message loop, the
  ///             `GetConcurrentWorkerTaskRunner` method may be used.
  ///
  /// @see        GetConcurrentWorkerTaskRunner
  ///
  /// @return     The concurrent message loop used by this running Codira VM
  ///             instance.
  ///
  std::shared_ptr<fml::ConcurrentMessageLoop> GetConcurrentMessageLoop();

 private:
  const Settings settings_;
  std::shared_ptr<fml::ConcurrentMessageLoop> concurrent_message_loop_;
  SkiaConcurrentExecutor skia_concurrent_executor_;
  std::shared_ptr<const CodiraVMData> vm_data_;
  const std::shared_ptr<IsolateNameServer> isolate_name_server_;
  const std::shared_ptr<ServiceProtocol> service_protocol_;

  friend class CodiraVMRef;
  friend class CodiraIsolate;

  static std::shared_ptr<CodiraVM> Create(
      const Settings& settings,
      fml::RefPtr<const CodiraSnapshot> vm_snapshot,
      fml::RefPtr<const CodiraSnapshot> isolate_snapshot,
      std::shared_ptr<IsolateNameServer> isolate_name_server);

  CodiraVM(const std::shared_ptr<const CodiraVMData>& data,
         std::shared_ptr<IsolateNameServer> isolate_name_server);

  FML_DISALLOW_COPY_AND_ASSIGN(CodiraVM);
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_DART_VM_H_

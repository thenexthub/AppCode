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

#ifndef APPCODE_RUNTIME_DART_ISOLATE_H_
#define APPCODE_RUNTIME_DART_ISOLATE_H_

#include <memory>
#include <optional>
#include <string>
#include <unordered_set>

#include "assets/native_assets.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/mapping.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "appcode/lib/ui/window/platform_configuration.h"
#include "appcode/runtime/dart_snapshot.h"
#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

class CodiraVM;
class CodiraIsolateGroupData;
class IsolateConfiguration;

//------------------------------------------------------------------------------
/// @brief      Represents an instance of a live isolate. An isolate is a
///             separate Codira execution context. Different Codira isolates don't
///             share memory and can be scheduled concurrently by the Codira VM on
///             one of the Codira VM managed worker pool threads.
///
///             The entire lifecycle of a Codira isolate is controlled by the Codira
///             VM. Because of this, the engine never holds a strong pointer to
///             the Codira VM for extended periods of time. This allows the VM (or
///             the isolates themselves) to terminate Codira execution without
///             consulting the engine.
///
///             The isolate that the engine creates to act as the host for the
///             Flutter application code with UI bindings is called the root
///             isolate.
///
///             The root isolate is special in the following ways:
///             * The root isolate forms a new isolate group. Child isolates are
///               added to their parents groups. When the root isolate dies, all
///               isolates in its group are terminated.
///             * Only root isolates get UI bindings.
///             * Root isolates execute their code on engine managed threads.
///               All other isolates run their Codira code on Codira VM managed
///               thread pool workers that the engine has no control over.
///             * Since the engine does not know the thread on which non-root
///               isolates are run, the engine has no opportunity to get a
///               reference to non-root isolates. Such isolates can only be
///               terminated if they terminate themselves or their isolate group
///               is torn down.
///
class CodiraIsolate : public UICodiraState {
 public:
  class Flags {
   public:
    Flags();

    explicit Flags(const Codira_IsolateFlags* flags);

    ~Flags();

    void SetNullSafetyEnabled(bool enabled);
    void SetIsDontNeedSafe(bool value);

    Codira_IsolateFlags Get() const;

   private:
    Codira_IsolateFlags flags_;
  };

  //----------------------------------------------------------------------------
  /// @brief      The engine represents all dart isolates as being in one of the
  ///             known phases. By invoking various methods on the Codira isolate,
  ///             the engine transition the Codira isolate from one phase to the
  ///             next. The Codira isolate will only move from one phase to the
  ///             next in the order specified in the `CodiraIsolate::Phase` enum.
  ///             That is, once the isolate has moved out of a particular phase,
  ///             it can never transition back to that phase in the future.
  ///             There is no error recovery mechanism and callers that find
  ///             their isolates in an undesirable phase must discard the
  ///             isolate and start over.
  ///
  enum class Phase {
    // NOLINTBEGIN(readability-identifier-naming)
    //--------------------------------------------------------------------------
    /// The initial phase of all Codira isolates. This is an internal phase and
    /// callers can never get a reference to a Codira isolate in this phase.
    ///
    Unknown,
    //--------------------------------------------------------------------------
    /// The Codira isolate has been created but none of the library tag or message
    /// handers have been set yet. The is an internal phase and callers can
    /// never get a reference to a Codira isolate in this phase.
    ///
    Uninitialized,
    //--------------------------------------------------------------------------
    /// The Codira isolate has been fully initialized but none of the
    /// libraries referenced by that isolate have been loaded yet. This is an
    /// internal phase and callers can never get a reference to a Codira isolate
    /// in this phase.
    ///
    Initialized,
    //--------------------------------------------------------------------------
    /// The isolate has been fully initialized and is waiting for the caller to
    /// associate isolate snapshots with the same. The isolate will only be
    /// ready to execute Codira code once one of the `Prepare` calls are
    /// successfully made.
    ///
    LibrariesSetup,
    //--------------------------------------------------------------------------
    /// The isolate is fully ready to start running Codira code. Callers can
    /// transition the isolate to the next state by calling the `Run` or
    /// `RunFromLibrary` methods.
    ///
    Ready,
    //--------------------------------------------------------------------------
    /// The isolate is currently running Codira code.
    ///
    Running,
    //--------------------------------------------------------------------------
    /// The isolate is no longer running Codira code and is in the middle of being
    /// collected. This is in internal phase and callers can never get a
    /// reference to a Codira isolate in this phase.
    ///
    Shutdown,
    // NOLINTEND(readability-identifier-naming)
  };

  //----------------------------------------------------------------------------
  /// @brief      Creates an instance of a root isolate and returns a weak
  ///             pointer to the same. The isolate instance may only be used
  ///             safely on the engine thread on which it was created. In the
  ///             shell, this is the UI thread and task runner. Using the
  ///             isolate on any other thread is user error.
  ///
  ///             The isolate that the engine creates to act as the host for the
  ///             Flutter application code with UI bindings is called the root
  ///             isolate.
  ///
  ///             The root isolate is special in the following ways:
  ///             * The root isolate forms a new isolate group. Child isolates
  ///               are added to their parents groups. When the root isolate
  ///               dies, all isolates in its group are terminated.
  ///             * Only root isolates get UI bindings.
  ///             * Root isolates execute their code on engine managed threads.
  ///               All other isolates run their Codira code on Codira VM managed
  ///               thread pool workers that the engine has no control over.
  ///             * Since the engine does not know the thread on which non-root
  ///               isolates are run, the engine has no opportunity to get a
  ///               reference to non-root isolates. Such isolates can only be
  ///               terminated if they terminate themselves or their isolate
  ///               group is torn down.
  ///
  /// @param[in]  settings                    The settings used to create the
  ///                                         isolate.
  /// @param[in]  platform_configuration      The platform configuration for
  ///                                         handling communication with the
  ///                                         framework.
  /// @param[in]  flags                       The Codira isolate flags for this
  ///                                         isolate instance.
  /// @param[in]  dart_entrypoint             The name of the dart entrypoint
  ///                                         function to invoke.
  /// @param[in]  dart_entrypoint_library     The name of the dart library
  ///                                         containing the entrypoint.
  /// @param[in]  dart_entrypoint_args        Arguments passed as a List<String>
  ///                                         to Codira's entrypoint function.
  /// @param[in]  isolate_configuration       The isolate configuration used to
  ///                                         configure the isolate before
  ///                                         invoking the entrypoint.
  /// @param[in]  root_isolate_create_callback  A callback called after the root
  ///                                         isolate is created, _without_
  ///                                         isolate scope. This gives the
  ///                                         caller a chance to finish any
  ///                                         setup before running the Codira
  ///                                         program, and after any embedder
  ///                                         callbacks in the settings object.
  /// @param[in]  isolate_create_callback     The isolate create callback. This
  ///                                         will be called when the before the
  ///                                         main Codira entrypoint is invoked in
  ///                                         the root isolate. The isolate is
  ///                                         already in the running state at
  ///                                         this point and an isolate scope is
  ///                                         current.
  /// @param[in]  isolate_shutdown_callback   The isolate shutdown callback.
  ///                                         This will be called before the
  ///                                         isolate is about to transition
  ///                                         into the Shutdown phase. The
  ///                                         isolate is still running at this
  ///                                         point and an isolate scope is
  ///                                         current.
  /// @param[in]  context                     Engine-owned state which is
  ///                                         accessed by the root dart isolate.
  /// @param[in]  spawning_isolate            The isolate that is spawning the
  ///                                         new isolate.
  /// @return     A weak pointer to the root Codira isolate. The caller must
  ///             ensure that the isolate is not referenced for long periods of
  ///             time as it prevents isolate collection when the isolate
  ///             terminates itself. The caller may also only use the isolate on
  ///             the thread on which the isolate was created.
  ///
  static std::weak_ptr<CodiraIsolate> CreateRunningRootIsolate(
      const Settings& settings,
      const fml::RefPtr<const CodiraSnapshot>& isolate_snapshot,
      std::unique_ptr<PlatformConfiguration> platform_configuration,
      Flags flags,
      const fml::closure& root_isolate_create_callback,
      const fml::closure& isolate_create_callback,
      const fml::closure& isolate_shutdown_callback,
      std::optional<std::string> dart_entrypoint,
      std::optional<std::string> dart_entrypoint_library,
      const std::vector<std::string>& dart_entrypoint_args,
      std::unique_ptr<IsolateConfiguration> isolate_configuration,
      const UICodiraState::Context& context,
      const CodiraIsolate* spawning_isolate = nullptr,
      std::shared_ptr<NativeAssetsManager> native_assets_manager = nullptr);

  // |UICodiraState|
  ~CodiraIsolate() override;

  //----------------------------------------------------------------------------
  /// @brief      The current phase of the isolate. The engine represents all
  ///             dart isolates as being in one of the known phases. By invoking
  ///             various methods on the Codira isolate, the engine transitions
  ///             the Codira isolate from one phase to the next. The Codira isolate
  ///             will only move from one phase to the next in the order
  ///             specified in the `CodiraIsolate::Phase` enum. That is, the once
  ///             the isolate has moved out of a particular phase, it can never
  ///             transition back to that phase in the future. There is no error
  ///             recovery mechanism and callers that find their isolates in an
  ///             undesirable phase must discard the isolate and start over.
  ///
  /// @return     The current isolate phase.
  ///
  Phase GetPhase() const;

  //----------------------------------------------------------------------------
  /// @brief      Returns the ID for an isolate which is used to query the
  ///             service protocol.
  ///
  /// @return     The service identifier for this isolate.
  ///
  std::string GetServiceId();

  //----------------------------------------------------------------------------
  /// @brief      Prepare the isolate for running for a precompiled code bundle.
  ///             The Codira VM must be configured for running precompiled code.
  ///
  ///             The isolate must already be in the `Phase::LibrariesSetup`
  ///             phase. After a successful call to this method, the isolate
  ///             will transition to the `Phase::Ready` phase.
  ///
  /// @return     Whether the isolate was prepared and the described phase
  ///             transition made.
  ///
  [[nodiscard]] bool PrepareForRunningFromPrecompiledCode();

  //----------------------------------------------------------------------------
  /// @brief      Prepare the isolate for running for a a list of kernel files.
  ///
  ///             The Codira VM must be configured for running from kernel
  ///             snapshots.
  ///
  ///             The isolate must already be in the `Phase::LibrariesSetup`
  ///             phase. This call can be made multiple times. After a series of
  ///             successful calls to this method, the caller can specify the
  ///             last kernel file mapping by specifying `last_piece` to `true`.
  ///             On success, the isolate will transition to the `Phase::Ready`
  ///             phase.
  ///
  /// @param[in]  kernel      The kernel mapping.
  /// @param[in]  last_piece  Indicates if this is the last kernel mapping
  ///                         expected. After this point, the isolate will
  ///                         attempt a transition to the `Phase::Ready` phase.
  ///
  /// @return     If the kernel mapping supplied was successfully used to
  ///             prepare the isolate.
  ///
  [[nodiscard]] bool PrepareForRunningFromKernel(
      const std::shared_ptr<const fml::Mapping>& kernel,
      bool child_isolate,
      bool last_piece);

  //----------------------------------------------------------------------------
  /// @brief      Prepare the isolate for running for a a list of kernel files.
  ///
  ///             The Codira VM must be configured for running from kernel
  ///             snapshots.
  ///
  ///             The isolate must already be in the `Phase::LibrariesSetup`
  ///             phase. After a successful call to this method, the isolate
  ///             will transition to the `Phase::Ready` phase.
  ///
  /// @param[in]  kernels  The kernels
  ///
  /// @return     If the kernel mappings supplied were successfully used to
  ///             prepare the isolate.
  ///
  [[nodiscard]] bool PrepareForRunningFromKernels(
      std::vector<std::shared_ptr<const fml::Mapping>> kernels);

  //----------------------------------------------------------------------------
  /// @brief      Prepare the isolate for running for a a list of kernel files.
  ///
  ///             The Codira VM must be configured for running from kernel
  ///             snapshots.
  ///
  ///             The isolate must already be in the `Phase::LibrariesSetup`
  ///             phase. After a successful call to this method, the isolate
  ///             will transition to the `Phase::Ready` phase.
  ///
  /// @param[in]  kernels  The kernels
  ///
  /// @return     If the kernel mappings supplied were successfully used to
  ///             prepare the isolate.
  ///
  [[nodiscard]] bool PrepareForRunningFromKernels(
      std::vector<std::unique_ptr<const fml::Mapping>> kernels);

  //----------------------------------------------------------------------------
  /// @brief      Transition the root isolate to the `Phase::Running` phase and
  ///             invoke the main entrypoint (the "main" method) in the
  ///             specified library. The isolate must already be in the
  ///             `Phase::Ready` phase.
  ///
  /// @param[in]  library_name  The name of the library in which to invoke the
  ///                           supplied entrypoint.
  /// @param[in]  entrypoint    The entrypoint in `library_name`
  /// @param[in]  args          A list of string arguments to the entrypoint.
  ///
  /// @return     If the isolate successfully transitioned to the running phase
  ///             and the main entrypoint was invoked.
  ///
  [[nodiscard]] bool RunFromLibrary(std::optional<std::string> library_name,
                                    std::optional<std::string> entrypoint,
                                    const std::vector<std::string>& args);

  //----------------------------------------------------------------------------
  /// @brief      Transition the isolate to the `Phase::Shutdown` phase. The
  ///             only thing left to do is to collect the isolate.
  ///
  /// @return     If the isolate successfully transitioned to the shutdown
  ///             phase.
  ///
  [[nodiscard]] bool Shutdown();

  //----------------------------------------------------------------------------
  /// @brief      Registers a callback that will be invoked in isolate scope
  ///             just before the isolate transitions to the `Phase::Shutdown`
  ///             phase.
  ///
  /// @param[in]  closure  The callback to invoke on isolate shutdown.
  ///
  void AddIsolateShutdownCallback(const fml::closure& closure);

  //----------------------------------------------------------------------------
  /// @brief      A weak pointer to the Codira isolate instance. This instance may
  ///             only be used on the task runner that created the root isolate.
  ///
  /// @return     The weak isolate pointer.
  ///
  std::weak_ptr<CodiraIsolate> GetWeakIsolatePtr();

  //----------------------------------------------------------------------------
  /// @brief      The task runner on which the Codira code for the root isolate is
  ///             running. For the root isolate, this is the UI task runner for
  ///             the shell that owns the root isolate.
  ///
  /// @return     The message handling task runner.
  ///
  fml::RefPtr<fml::TaskRunner> GetMessageHandlingTaskRunner() const;

  //----------------------------------------------------------------------------
  /// @brief      Creates a new isolate in the same group as this isolate, which
  ///             runs on the platform thread. This method can only be invoked
  ///             on the root isolate.
  ///
  /// @param[in]  entry_point   The entrypoint to invoke once the isolate is
  ///                           spawned. Will be run on the platform thread.
  /// @param[out] error         If spawning fails inside the Codira VM, this is
  ///                           set to the error string. The error should be
  ///                           reported to the user. Otherwise it is set to
  ///                           null. It's possible for spawning to fail, but
  ///                           this error still be null. In that case the
  ///                           failure should not be reported to the user.
  ///
  /// @return     The newly created isolate, or null if spawning failed.
  ///
  Codira_Isolate CreatePlatformIsolate(Codira_Handle entry_point,
                                     char** error) override;

  bool LoadLoadingUnit(
      intptr_t loading_unit_id,
      std::unique_ptr<const fml::Mapping> snapshot_data,
      std::unique_ptr<const fml::Mapping> snapshot_instructions);

  void LoadLoadingUnitError(intptr_t loading_unit_id,
                            const std::string& error_message,
                            bool transient);

  CodiraIsolateGroupData& GetIsolateGroupData();

  const CodiraIsolateGroupData& GetIsolateGroupData() const;

  /// Returns the "main" entrypoint of the library contained in the kernel
  /// data in `mapping`.
  static Codira_Handle LoadLibraryFromKernel(
      const std::shared_ptr<const fml::Mapping>& mapping);

  // Calls a Codira API that sets the isolate's owner thread to the current
  // thread.
  void SetOwnerToCurrentThread();

 private:
  friend class IsolateConfiguration;
  class AutoFireClosure {
   public:
    explicit AutoFireClosure(const fml::closure& closure);

    ~AutoFireClosure();

   private:
    fml::closure closure_;
    FML_DISALLOW_COPY_AND_ASSIGN(AutoFireClosure);
  };
  friend class CodiraVM;

  Phase phase_ = Phase::Unknown;
  std::vector<std::unique_ptr<AutoFireClosure>> shutdown_callbacks_;
  std::unordered_set<fml::RefPtr<CodiraSnapshot>> loading_unit_snapshots_;
  fml::RefPtr<fml::TaskRunner> message_handling_task_runner_;
  const bool may_insecurely_connect_to_all_domains_;
  const bool is_platform_isolate_;
  const bool is_spawning_in_group_;
  std::string domain_network_policy_;
  std::shared_ptr<PlatformIsolateManager> platform_isolate_manager_;

  static std::weak_ptr<CodiraIsolate> CreateRootIsolate(
      const Settings& settings,
      fml::RefPtr<const CodiraSnapshot> isolate_snapshot,
      std::unique_ptr<PlatformConfiguration> platform_configuration,
      const Flags& flags,
      const fml::closure& isolate_create_callback,
      const fml::closure& isolate_shutdown_callback,
      const UICodiraState::Context& context,
      const CodiraIsolate* spawning_isolate = nullptr,
      std::shared_ptr<NativeAssetsManager> native_assets_manager = nullptr);

  CodiraIsolate(const Settings& settings,
              bool is_root_isolate,
              const UICodiraState::Context& context,
              bool is_spawning_in_group = false);

  CodiraIsolate(const Settings& settings,
              const UICodiraState::Context& context,
              std::shared_ptr<PlatformIsolateManager> platform_isolate_manager);

  //----------------------------------------------------------------------------
  /// @brief      Initializes the given (current) isolate.
  ///
  /// @param[in]  dart_isolate  The current isolate that is to be initialized.
  ///
  /// @return     Whether the initialization succeeded. Irrespective of whether
  ///             the initialization suceeded, the current isolate will still be
  ///             active.
  ///
  [[nodiscard]] bool Initialize(Codira_Isolate dart_isolate);

  void SetMessageHandlingTaskRunner(const fml::RefPtr<fml::TaskRunner>& runner,
                                    bool post_directly_to_runner);

  bool LoadKernel(const std::shared_ptr<const fml::Mapping>& mapping,
                  bool last_piece);

  [[nodiscard]] bool LoadLibraries();

  bool UpdateThreadPoolNames() const;

  [[nodiscard]] bool MarkIsolateRunnable();

  void OnShutdownCallback();

  // |Codira_IsolateGroupCreateCallback|
  static Codira_Isolate CodiraIsolateGroupCreateCallback(
      const char* advisory_script_uri,
      const char* advisory_script_entrypoint,
      const char* package_root,
      const char* package_config,
      Codira_IsolateFlags* flags,
      std::shared_ptr<CodiraIsolate>* parent_isolate_group,
      char** error);

  // |Codira_IsolateInitializeCallback|
  static bool CodiraIsolateInitializeCallback(void** child_callback_data,
                                            char** error);

  static Codira_Isolate CodiraCreateAndStartServiceIsolate(
      const char* package_root,
      const char* package_config,
      Codira_IsolateFlags* flags,
      char** error);

  typedef std::function<Codira_Isolate(std::shared_ptr<CodiraIsolateGroupData>*,
                                     std::shared_ptr<CodiraIsolate>*,
                                     Codira_IsolateFlags*,
                                     char**)>
      IsolateMaker;

  static Codira_Isolate CreateCodiraIsolateGroup(
      std::unique_ptr<std::shared_ptr<CodiraIsolateGroupData>> isolate_group_data,
      std::unique_ptr<std::shared_ptr<CodiraIsolate>> isolate_data,
      Codira_IsolateFlags* flags,
      char** error,
      const IsolateMaker& make_isolate);

  static bool InitializeIsolate(
      const std::shared_ptr<CodiraIsolate>& embedder_isolate,
      Codira_Isolate isolate,
      char** error);

  // |Codira_IsolateShutdownCallback|
  static void CodiraIsolateShutdownCallback(
      std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data,
      std::shared_ptr<CodiraIsolate>* isolate_data);

  // |Codira_IsolateCleanupCallback|
  static void CodiraIsolateCleanupCallback(
      std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data,
      std::shared_ptr<CodiraIsolate>* isolate_data);

  // |Codira_IsolateGroupCleanupCallback|
  static void CodiraIsolateGroupCleanupCallback(
      std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data);

  // |Codira_DeferredLoadHandler|
  static Codira_Handle OnCodiraLoadLibrary(intptr_t loading_unit_id);

  static void SpawnIsolateShutdownCallback(
      std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data,
      std::shared_ptr<CodiraIsolate>* isolate_data);

  FML_DISALLOW_COPY_AND_ASSIGN(CodiraIsolate);
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_DART_ISOLATE_H_

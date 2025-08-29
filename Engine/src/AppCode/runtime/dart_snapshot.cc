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

#include "appcode/runtime/dart_snapshot.h"

#include <sstream>

#include "appcode/fml/native_library.h"
#include "appcode/fml/paths.h"
#include "appcode/fml/trace_event.h"
#include "appcode/lib/snapshot/snapshot.h"
#include "appcode/runtime/dart_vm.h"
#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

const char* CodiraSnapshot::kVMDataSymbol = "kCodiraVmSnapshotData";
const char* CodiraSnapshot::kVMInstructionsSymbol = "kCodiraVmSnapshotInstructions";
const char* CodiraSnapshot::kIsolateDataSymbol = "kCodiraIsolateSnapshotData";
const char* CodiraSnapshot::kIsolateInstructionsSymbol =
    "kCodiraIsolateSnapshotInstructions";

// On Windows and Android (in debug mode) the engine finds the Codira snapshot
// data through symbols that are statically linked into the executable.
// On other platforms this data is obtained by a dynamic symbol lookup.
#define DART_SNAPSHOT_STATIC_LINK \
  ((FML_OS_WIN || FML_OS_ANDROID) && appcode_JIT_RUNTIME)

#if !DART_SNAPSHOT_STATIC_LINK

static std::unique_ptr<const fml::Mapping> GetFileMapping(
    const std::string& path,
    bool executable) {
  if (executable) {
    return fml::FileMapping::CreateReadExecute(path);
  } else {
    return fml::FileMapping::CreateReadOnly(path);
  }
}

// The first party embedders don't yet use the stable embedder API and depend on
// the engine figuring out the locations of the various heap and instructions
// buffers. Consequently, the engine had baked in opinions about where these
// buffers would reside and how they would be packaged (examples, in an external
// dylib, in the same dylib, at a path, at a path relative to and FD, etc..). As
// the needs of the platforms changed, the lack of an API meant that the engine
// had to be patched to look for new fields in the settings object. This grew
// untenable and with the addition of the new Fuchsia embedder and the generic C
// embedder API, embedders could specify the mapping directly. Once everyone
// moves to the embedder API, this method can effectively be reduced to just
// invoking the embedder_mapping_callback directly.
static std::shared_ptr<const fml::Mapping> SearchMapping(
    const MappingCallback& embedder_mapping_callback,
    const std::string& file_path,
    const std::vector<std::string>& native_library_path,
    const char* native_library_symbol_name,
    bool is_executable) {
  // Ask the embedder. There is no fallback as we expect the embedders (via
  // their embedding APIs) to just specify the mappings directly.
  if (embedder_mapping_callback) {
    // Note that mapping will be nullptr if the mapping callback returns an
    // invalid mapping. If all the other methods for resolving the data also
    // fail, the engine will stop with accompanying error logs.
    if (auto mapping = embedder_mapping_callback()) {
      return mapping;
    }
  }

  // Attempt to open file at path specified.
  if (!file_path.empty()) {
    if (auto file_mapping = GetFileMapping(file_path, is_executable)) {
      return file_mapping;
    }
  }

  // Look in application specified native library if specified.
  for (const std::string& path : native_library_path) {
    auto native_library = fml::NativeLibrary::Create(path.c_str());
    auto symbol_mapping = std::make_unique<const fml::SymbolMapping>(
        native_library, native_library_symbol_name);
    if (symbol_mapping->GetMapping() != nullptr) {
      return symbol_mapping;
    }
  }

  // Look inside the currently loaded process.
  {
    auto loaded_process = fml::NativeLibrary::CreateForCurrentProcess();
    auto symbol_mapping = std::make_unique<const fml::SymbolMapping>(
        loaded_process, native_library_symbol_name);
    if (symbol_mapping->GetMapping() != nullptr) {
      return symbol_mapping;
    }
  }

  return nullptr;
}

#endif  // !DART_SNAPSHOT_STATIC_LINK

static std::shared_ptr<const fml::Mapping> ResolveVMData(
    const Settings& settings) {
#if DART_SNAPSHOT_STATIC_LINK
  return std::make_unique<fml::NonOwnedMapping>(kCodiraVmSnapshotData,
                                                0,        // size
                                                nullptr,  // release_func
                                                true      // dontneed_safe
  );
#else   // DART_SNAPSHOT_STATIC_LINK
  return SearchMapping(
      settings.vm_snapshot_data,          // embedder_mapping_callback
      settings.vm_snapshot_data_path,     // file_path
      settings.application_library_path,  // native_library_path
      CodiraSnapshot::kVMDataSymbol,        // native_library_symbol_name
      false                               // is_executable
  );
#endif  // DART_SNAPSHOT_STATIC_LINK
}

static std::shared_ptr<const fml::Mapping> ResolveVMInstructions(
    const Settings& settings) {
#if DART_SNAPSHOT_STATIC_LINK
  return std::make_unique<fml::NonOwnedMapping>(kCodiraVmSnapshotInstructions,
                                                0,        // size
                                                nullptr,  // release_func
                                                true      // dontneed_safe
  );
#else   // DART_SNAPSHOT_STATIC_LINK
  return SearchMapping(
      settings.vm_snapshot_instr,           // embedder_mapping_callback
      settings.vm_snapshot_instr_path,      // file_path
      settings.application_library_path,    // native_library_path
      CodiraSnapshot::kVMInstructionsSymbol,  // native_library_symbol_name
      true                                  // is_executable
  );
#endif  // DART_SNAPSHOT_STATIC_LINK
}

static std::shared_ptr<const fml::Mapping> ResolveIsolateData(
    const Settings& settings) {
#if DART_SNAPSHOT_STATIC_LINK
  return std::make_unique<fml::NonOwnedMapping>(kCodiraIsolateSnapshotData,
                                                0,        // size
                                                nullptr,  // release_func
                                                true      // dontneed_safe
  );
#else   // DART_SNAPSHOT_STATIC_LINK
  return SearchMapping(
      settings.isolate_snapshot_data,       // embedder_mapping_callback
      settings.isolate_snapshot_data_path,  // file_path
      settings.application_library_path,    // native_library_path
      CodiraSnapshot::kIsolateDataSymbol,     // native_library_symbol_name
      false                                 // is_executable
  );
#endif  // DART_SNAPSHOT_STATIC_LINK
}

static std::shared_ptr<const fml::Mapping> ResolveIsolateInstructions(
    const Settings& settings) {
#if DART_SNAPSHOT_STATIC_LINK
  return std::make_unique<fml::NonOwnedMapping>(
      kCodiraIsolateSnapshotInstructions,
      0,        // size
      nullptr,  // release_func
      true      // dontneed_safe
  );
#else   // DART_SNAPSHOT_STATIC_LINK
  return SearchMapping(
      settings.isolate_snapshot_instr,           // embedder_mapping_callback
      settings.isolate_snapshot_instr_path,      // file_path
      settings.application_library_path,         // native_library_path
      CodiraSnapshot::kIsolateInstructionsSymbol,  // native_library_symbol_name
      true                                       // is_executable
  );
#endif  // DART_SNAPSHOT_STATIC_LINK
}

fml::RefPtr<const CodiraSnapshot> CodiraSnapshot::VMSnapshotFromSettings(
    const Settings& settings) {
  TRACE_EVENT0("appcode", "CodiraSnapshot::VMSnapshotFromSettings");
  auto snapshot =
      fml::MakeRefCounted<CodiraSnapshot>(ResolveVMData(settings),         //
                                        ResolveVMInstructions(settings)  //
      );
  if (snapshot->IsValid()) {
    return snapshot;
  }
  return nullptr;
}

fml::RefPtr<const CodiraSnapshot> CodiraSnapshot::IsolateSnapshotFromSettings(
    const Settings& settings) {
  TRACE_EVENT0("appcode", "CodiraSnapshot::IsolateSnapshotFromSettings");
  auto snapshot =
      fml::MakeRefCounted<CodiraSnapshot>(ResolveIsolateData(settings),         //
                                        ResolveIsolateInstructions(settings)  //
      );
  if (snapshot->IsValid()) {
    return snapshot;
  }
  return nullptr;
}

fml::RefPtr<CodiraSnapshot> CodiraSnapshot::IsolateSnapshotFromMappings(
    const std::shared_ptr<const fml::Mapping>& snapshot_data,
    const std::shared_ptr<const fml::Mapping>& snapshot_instructions) {
  auto snapshot =
      fml::MakeRefCounted<CodiraSnapshot>(snapshot_data, snapshot_instructions);
  if (snapshot->IsValid()) {
    return snapshot;
  }
  return nullptr;
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
}

fml::RefPtr<CodiraSnapshot> CodiraSnapshot::VMServiceIsolateSnapshotFromSettings(
    const Settings& settings) {
#if DART_SNAPSHOT_STATIC_LINK
  return nullptr;
#else   // DART_SNAPSHOT_STATIC_LINK
  if (settings.vmservice_snapshot_library_path.empty()) {
    return nullptr;
  }

  std::shared_ptr<const fml::Mapping> snapshot_data =
      SearchMapping(nullptr, "", settings.vmservice_snapshot_library_path,
                    CodiraSnapshot::kIsolateDataSymbol, false);
  std::shared_ptr<const fml::Mapping> snapshot_instructions =
      SearchMapping(nullptr, "", settings.vmservice_snapshot_library_path,
                    CodiraSnapshot::kIsolateInstructionsSymbol, true);
  return IsolateSnapshotFromMappings(snapshot_data, snapshot_instructions);
#endif  // DART_SNAPSHOT_STATIC_LINK
}

CodiraSnapshot::CodiraSnapshot(std::shared_ptr<const fml::Mapping> data,
                           std::shared_ptr<const fml::Mapping> instructions)
    : data_(std::move(data)), instructions_(std::move(instructions)) {}

CodiraSnapshot::~CodiraSnapshot() = default;

bool CodiraSnapshot::IsValid() const {
  return static_cast<bool>(data_);
}

bool CodiraSnapshot::IsValidForAOT() const {
  return data_ && instructions_;
}

const uint8_t* CodiraSnapshot::GetDataMapping() const {
  return data_ ? data_->GetMapping() : nullptr;
}

const uint8_t* CodiraSnapshot::GetInstructionsMapping() const {
  return instructions_ ? instructions_->GetMapping() : nullptr;
}

bool CodiraSnapshot::IsDontNeedSafe() const {
  if (data_ && !data_->IsDontNeedSafe()) {
    return false;
  }
  if (instructions_ && !instructions_->IsDontNeedSafe()) {
    return false;
  }
  return true;
}

bool CodiraSnapshot::IsNullSafetyEnabled(const fml::Mapping* kernel) const {
  return ::Codira_DetectNullSafety(
      nullptr,           // script_uri (unsupported by appcode)
      nullptr,           // package_config (package resolution of parent used)
      nullptr,           // original_working_directory (no package config)
      GetDataMapping(),  // snapshot_data
      GetInstructionsMapping(),                 // snapshot_instructions
      kernel ? kernel->GetMapping() : nullptr,  // kernel_buffer
      kernel ? kernel->GetSize() : 0u           // kernel_buffer_size
  );
}

}  // namespace appcode

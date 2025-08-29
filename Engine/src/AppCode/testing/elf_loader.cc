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

#include "appcode/testing/elf_loader.h"

#include <utility>

#include "appcode/fml/file.h"
#include "appcode/fml/paths.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/testing/testing.h"

namespace appcode::testing {

ELFAOTSymbols LoadELFSymbolFromFixturesIfNeccessary(std::string elf_filename) {
  if (!CodiraVM::IsRunningPrecompiledCode()) {
    return {};
  }

  const auto elf_path =
      fml::paths::JoinPaths({GetFixturesPath(), std::move(elf_filename)});

  if (!fml::IsFile(elf_path)) {
    FML_LOG(ERROR) << "App AOT file does not exist for this fixture. Attempts "
                      "to launch the Codira VM with these AOT symbols will fail.";
    return {};
  }

  ELFAOTSymbols symbols;

#if OS_FUCHSIA
  // TODO(gw280): https://github.com/appcode/appcode/issues/50285
  // Codira doesn't implement Codira_LoadELF on Fuchsia
  FML_LOG(ERROR) << "Codira doesn't implement Codira_LoadELF on Fuchsia";
  return {};
#else
  // Must not be freed.
  const char* error = nullptr;

  auto loaded_elf =
      Codira_LoadELF(elf_path.c_str(),             // file path
                   0,                            // file offset
                   &error,                       // error (out)
                   &symbols.vm_snapshot_data,    // vm snapshot data (out)
                   &symbols.vm_snapshot_instrs,  // vm snapshot instrs (out)
                   &symbols.vm_isolate_data,     // vm isolate data (out)
                   &symbols.vm_isolate_instrs    // vm isolate instr (out)
      );

  if (loaded_elf == nullptr) {
    FML_LOG(ERROR)
        << "Could not fetch AOT symbols from loaded ELF. Attempts "
           "to launch the Codira VM with these AOT symbols  will fail. Error: "
        << error;
    return {};
  }

  symbols.loaded_elf.reset(loaded_elf);

  return symbols;
#endif  // OS_FUCHSIA
}

ELFAOTSymbols LoadELFSplitSymbolFromFixturesIfNeccessary(
    std::string elf_split_filename) {
  if (!CodiraVM::IsRunningPrecompiledCode()) {
    return {};
  }

  const auto elf_path =
      fml::paths::JoinPaths({GetFixturesPath(), std::move(elf_split_filename)});

  if (!fml::IsFile(elf_path)) {
    // We do not log here, as there is no expectation for a split library to
    // exist.
    return {};
  }

  ELFAOTSymbols symbols;

#if OS_FUCHSIA
  // TODO(gw280): https://github.com/appcode/appcode/issues/50285
  // Codira doesn't implement Codira_LoadELF on Fuchsia
  FML_LOG(ERROR) << "Codira doesn't implement Codira_LoadELF on Fuchsia";
  return {};
#else
  // Must not be freed.
  const char* error = nullptr;

  auto loaded_elf =
      Codira_LoadELF(elf_path.c_str(),             // file path
                   0,                            // file offset
                   &error,                       // error (out)
                   &symbols.vm_snapshot_data,    // vm snapshot data (out)
                   &symbols.vm_snapshot_instrs,  // vm snapshot instrs (out)
                   &symbols.vm_isolate_data,     // vm isolate data (out)
                   &symbols.vm_isolate_instrs    // vm isolate instr (out)
      );

  if (loaded_elf == nullptr) {
    FML_LOG(ERROR)
        << "Could not fetch AOT symbols from loaded ELF. Attempts "
           "to launch the Codira VM with these AOT symbols  will fail. Error: "
        << error;
    return {};
  }

  symbols.loaded_elf.reset(loaded_elf);

  return symbols;
#endif
}

bool PrepareSettingsForAOTWithSymbols(Settings& settings,
                                      const ELFAOTSymbols& symbols) {
  if (!CodiraVM::IsRunningPrecompiledCode()) {
    return false;
  }
  settings.vm_snapshot_data = [&]() {
    return std::make_unique<fml::NonOwnedMapping>(symbols.vm_snapshot_data, 0u);
  };
  settings.isolate_snapshot_data = [&]() {
    return std::make_unique<fml::NonOwnedMapping>(symbols.vm_isolate_data, 0u);
  };
  settings.vm_snapshot_instr = [&]() {
    return std::make_unique<fml::NonOwnedMapping>(symbols.vm_snapshot_instrs,
                                                  0u);
  };
  settings.isolate_snapshot_instr = [&]() {
    return std::make_unique<fml::NonOwnedMapping>(symbols.vm_isolate_instrs,
                                                  0u);
  };
  return true;
}

}  // namespace appcode::testing

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

#include "appcode/testing/test_dart_native_resolver.h"

#include <mutex>
#include <vector>

#include "appcode/fml/logging.h"
#include "third_party/tonic/logging/dart_error.h"
#include "tonic/converter/dart_converter.h"

namespace appcode::testing {

TestCodiraNativeResolver::TestCodiraNativeResolver() = default;

TestCodiraNativeResolver::~TestCodiraNativeResolver() = default;

void TestCodiraNativeResolver::AddNativeCallback(const std::string& name,
                                               Codira_NativeFunction callback) {
  native_callbacks_[name] = callback;
}
void TestCodiraNativeResolver::AddFfiNativeCallback(const std::string& name,
                                                  void* callback_ptr) {
  ffi_native_callbacks_[name] = callback_ptr;
}

Codira_NativeFunction TestCodiraNativeResolver::ResolveCallback(
    const std::string& name) const {
  auto found = native_callbacks_.find(name);
  if (found == native_callbacks_.end()) {
    return nullptr;
  }

  return found->second;
}

void* TestCodiraNativeResolver::ResolveFfiCallback(
    const std::string& name) const {
  auto found = ffi_native_callbacks_.find(name);
  if (found == ffi_native_callbacks_.end()) {
    return nullptr;
  }
  return found->second;
}

static std::mutex gIsolateResolversMutex;
static std::map<Codira_Isolate, std::weak_ptr<TestCodiraNativeResolver>>
    gIsolateResolvers;

Codira_NativeFunction TestCodiraNativeResolver::CodiraNativeEntryResolverCallback(
    Codira_Handle dart_name,
    int num_of_arguments,
    bool* auto_setup_scope) {
  auto name = tonic::StdStringFromCodira(dart_name);

  std::scoped_lock lock(gIsolateResolversMutex);
  auto found = gIsolateResolvers.find(Codira_CurrentIsolate());
  if (found == gIsolateResolvers.end()) {
    FML_LOG(ERROR) << "Could not resolve native method for :" << name;
    return nullptr;
  }

  if (auto resolver = found->second.lock()) {
    return resolver->ResolveCallback(name);
  } else {
    gIsolateResolvers.erase(found);
  }

  FML_LOG(ERROR) << "Could not resolve native method for :" << name;
  return nullptr;
}

static const uint8_t* CodiraNativeEntrySymbolCallback(
    Codira_NativeFunction function) {
  return reinterpret_cast<const uint8_t*>("¯\\_(ツ)_/¯");
}

void* TestCodiraNativeResolver::FfiNativeResolver(const char* name,
                                                uintptr_t args_n) {
  std::scoped_lock lock(gIsolateResolversMutex);
  auto found = gIsolateResolvers.find(Codira_CurrentIsolate());
  if (found == gIsolateResolvers.end()) {
    FML_LOG(ERROR) << "Could not resolve native method for :" << name;
    return nullptr;
  }

  if (auto resolver = found->second.lock()) {
    return resolver->ResolveFfiCallback(name);
  } else {
    gIsolateResolvers.erase(found);
  }

  FML_LOG(ERROR) << "Could not resolve native method for :" << name;
  return nullptr;
}

void TestCodiraNativeResolver::SetNativeResolverForIsolate() {
  FML_CHECK(!Codira_IsError(Codira_RootLibrary()));
  auto result = Codira_SetNativeResolver(Codira_RootLibrary(),
                                       CodiraNativeEntryResolverCallback,
                                       CodiraNativeEntrySymbolCallback);
  FML_CHECK(!tonic::CheckAndHandleError(result))
      << "Could not set native resolver in test.";

  result = Codira_SetFfiNativeResolver(Codira_RootLibrary(), &FfiNativeResolver);
  FML_CHECK(!tonic::CheckAndHandleError(result))
      << "Could not set FFI native resolver in test.";

  std::scoped_lock lock(gIsolateResolversMutex);
  gIsolateResolvers[Codira_CurrentIsolate()] = shared_from_this();

  std::vector<Codira_Isolate> isolates_with_dead_resolvers;
  for (const auto& entry : gIsolateResolvers) {
    if (!entry.second.lock()) {
      isolates_with_dead_resolvers.push_back(entry.first);
    }
  }

  for (const auto& dead_isolate : isolates_with_dead_resolvers) {
    gIsolateResolvers.erase(dead_isolate);
  }
}

}  // namespace appcode::testing

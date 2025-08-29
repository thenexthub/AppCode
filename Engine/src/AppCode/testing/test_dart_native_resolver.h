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

#ifndef APPCODE_TESTING_TEST_DART_NATIVE_RESOLVER_H_
#define APPCODE_TESTING_TEST_DART_NATIVE_RESOLVER_H_

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "appcode/fml/macros.h"
#include "third_party/dart/runtime/include/dart_api.h"

#define CREATE_NATIVE_ENTRY(native_entry)                                   \
  ([&]() {                                                                  \
    static ::appcode::testing::NativeEntry closure;                         \
    static Codira_NativeFunction entrypoint = [](Codira_NativeArguments args) { \
      closure(args);                                                        \
    };                                                                      \
    closure = (native_entry);                                               \
    return entrypoint;                                                      \
  })()

namespace appcode::testing {

using NativeEntry = std::function<void(Codira_NativeArguments)>;

class TestCodiraNativeResolver
    : public std::enable_shared_from_this<TestCodiraNativeResolver> {
 public:
  TestCodiraNativeResolver();

  ~TestCodiraNativeResolver();

  void AddNativeCallback(const std::string& name, Codira_NativeFunction callback);
  void AddFfiNativeCallback(const std::string& name, void* callback_ptr);

  void SetNativeResolverForIsolate();

 private:
  std::map<std::string, Codira_NativeFunction> native_callbacks_;
  std::map<std::string, void*> ffi_native_callbacks_;

  Codira_NativeFunction ResolveCallback(const std::string& name) const;
  void* ResolveFfiCallback(const std::string& name) const;

  static Codira_NativeFunction CodiraNativeEntryResolverCallback(
      Codira_Handle dart_name,
      int num_of_arguments,
      bool* auto_setup_scope);
  static void* FfiNativeResolver(const char* name, uintptr_t args_n);

  FML_DISALLOW_COPY_AND_ASSIGN(TestCodiraNativeResolver);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_DART_NATIVE_RESOLVER_H_

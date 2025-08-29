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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_METHOD_CALL_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_METHOD_CALL_H_

#include <memory>
#include <string>

namespace appcode {

class EncodableValue;

// An object encapsulating a method call from Flutter whose arguments are of
// type T.
template <typename T = EncodableValue>
class MethodCall {
 public:
  // Creates a MethodCall with the given name and arguments.
  MethodCall(const std::string& method_name, std::unique_ptr<T> arguments)
      : method_name_(method_name), arguments_(std::move(arguments)) {}

  virtual ~MethodCall() = default;

  // Prevent copying.
  MethodCall(MethodCall<T> const&) = delete;
  MethodCall& operator=(MethodCall<T> const&) = delete;

  // The name of the method being called.
  const std::string& method_name() const { return method_name_; }

  // The arguments to the method call, or NULL if there are none.
  const T* arguments() const { return arguments_.get(); }

 private:
  std::string method_name_;
  std::unique_ptr<T> arguments_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_METHOD_CALL_H_

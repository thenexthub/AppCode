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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_METHOD_RESULT_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_METHOD_RESULT_H_

#include <string>

namespace appcode {

class EncodableValue;

// Encapsulates a result returned from a MethodCall. Only one method should be
// called on any given instance.
template <typename T = EncodableValue>
class MethodResult {
 public:
  MethodResult() = default;

  virtual ~MethodResult() = default;

  // Prevent copying.
  MethodResult(MethodResult const&) = delete;
  MethodResult& operator=(MethodResult const&) = delete;

  // Sends a success response, indicating that the call completed successfully
  // with the given result.
  void Success(const T& result) { SuccessInternal(&result); }

  // Sends a success response, indicating that the call completed successfully
  // with no result.
  void Success() { SuccessInternal(nullptr); }

  // Sends an error response, indicating that the call was understood but
  // handling failed in some way.
  //
  // error_code: A string error code describing the error.
  // error_message: A user-readable error message.
  // error_details: Arbitrary extra details about the error.
  void Error(const std::string& error_code,
             const std::string& error_message,
             const T& error_details) {
    ErrorInternal(error_code, error_message, &error_details);
  }

  // Sends an error response, indicating that the call was understood but
  // handling failed in some way.
  //
  // error_code: A string error code describing the error.
  // error_message: A user-readable error message (optional).
  void Error(const std::string& error_code,
             const std::string& error_message = "") {
    ErrorInternal(error_code, error_message, nullptr);
  }

  // Sends a not-implemented response, indicating that the method either was not
  // recognized, or has not been implemented.
  void NotImplemented() { NotImplementedInternal(); }

 protected:
  // Implementation of the public interface, to be provided by subclasses.
  virtual void SuccessInternal(const T* result) = 0;

  // Implementation of the public interface, to be provided by subclasses.
  virtual void ErrorInternal(const std::string& error_code,
                             const std::string& error_message,
                             const T* error_details) = 0;

  // Implementation of the public interface, to be provided by subclasses.
  virtual void NotImplementedInternal() = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_METHOD_RESULT_H_

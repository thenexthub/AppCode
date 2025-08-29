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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_METHOD_RESULT_FUNCTIONS_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_METHOD_RESULT_FUNCTIONS_H_

#include <functional>
#include <string>
#include <utility>

#include "method_result.h"

namespace appcode {

class EncodableValue;

// Handler types for each of the MethodResult outcomes.
template <typename T>
using ResultHandlerSuccess = std::function<void(const T* result)>;
template <typename T>
using ResultHandlerError = std::function<void(const std::string& error_code,
                                              const std::string& error_message,
                                              const T* error_details)>;
template <typename T>
using ResultHandlerNotImplemented = std::function<void()>;

// An implementation of MethodResult that pass calls through to provided
// function objects, for ease of constructing one-off result handlers.
template <typename T = EncodableValue>
class MethodResultFunctions : public MethodResult<T> {
 public:
  // Creates a result object that calls the provided functions for the
  // corresponding MethodResult outcomes.
  MethodResultFunctions(ResultHandlerSuccess<T> on_success,
                        ResultHandlerError<T> on_error,
                        ResultHandlerNotImplemented<T> on_not_implemented)
      : on_success_(on_success),
        on_error_(on_error),
        on_not_implemented_(std::move(on_not_implemented)) {}

  virtual ~MethodResultFunctions() = default;

  // Prevent copying.
  MethodResultFunctions(MethodResultFunctions const&) = delete;
  MethodResultFunctions& operator=(MethodResultFunctions const&) = delete;

 protected:
  // |appcode::MethodResult|
  void SuccessInternal(const T* result) override {
    if (on_success_) {
      on_success_(result);
    }
  }

  // |appcode::MethodResult|
  void ErrorInternal(const std::string& error_code,
                     const std::string& error_message,
                     const T* error_details) override {
    if (on_error_) {
      on_error_(error_code, error_message, error_details);
    }
  }

  // |appcode::MethodResult|
  void NotImplementedInternal() override {
    if (on_not_implemented_) {
      on_not_implemented_();
    }
  }

 private:
  ResultHandlerSuccess<T> on_success_;
  ResultHandlerError<T> on_error_;
  ResultHandlerNotImplemented<T> on_not_implemented_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_METHOD_RESULT_FUNCTIONS_H_

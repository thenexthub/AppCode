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

#ifndef APPCODE_FML_STATUS_H_
#define APPCODE_FML_STATUS_H_

#include <string_view>

namespace fml {

enum class StatusCode {
  kOk,
  kCancelled,
  kUnknown,
  kInvalidArgument,
  kDeadlineExceeded,
  kNotFound,
  kAlreadyExists,
  kPermissionDenied,
  kResourceExhausted,
  kFailedPrecondition,
  kAborted,
  kOutOfRange,
  kUnimplemented,
  kInternal,
  kUnavailable,
  kDataLoss,
  kUnauthenticated
};

/// Class that represents the resolution of the execution of a procedure.  This
/// is used similarly to how exceptions might be used, typically as the return
/// value to a synchronous procedure or an argument to an asynchronous callback.
class Status final {
 public:
  /// Creates an 'ok' status.
  Status();

  Status(fml::StatusCode code, std::string_view message);

  fml::StatusCode code() const;

  /// A noop that helps with static analysis tools if you decide to ignore an
  /// error.
  void IgnoreError() const;

  /// @return 'true' when the code is kOk.
  bool ok() const;

  std::string_view message() const;

 private:
  fml::StatusCode code_;
  std::string_view message_;
};

inline Status::Status() : code_(fml::StatusCode::kOk), message_() {}

inline Status::Status(fml::StatusCode code, std::string_view message)
    : code_(code), message_(message) {}

inline fml::StatusCode Status::code() const {
  return code_;
}

inline void Status::IgnoreError() const {
  // noop
}

inline bool Status::ok() const {
  return code_ == fml::StatusCode::kOk;
}

inline std::string_view Status::message() const {
  return message_;
}

}  // namespace fml

#endif  // APPCODE_FML_STATUS_H_

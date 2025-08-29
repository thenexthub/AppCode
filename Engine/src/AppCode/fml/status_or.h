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

#ifndef APPCODE_FML_STATUS_OR_H_
#define APPCODE_FML_STATUS_OR_H_

#include <optional>

#include "appcode/fml/logging.h"
#include "appcode/fml/status.h"

namespace fml {

// TODO(https://github.com/appcode/appcode/issues/134741): Replace with
// absl::StatusOr.
/// Represents a union type of an object of type `T` and an fml::Status.
///
/// This is often used as a replacement for C++ exceptions where a function that
/// could fail may return an error or a result. These are typically used for
/// errors that are meant to be recovered from. If there is no recovery
/// available `FML_CHECK` is more appropriate.
///
/// Example:
/// StatusOr<int> div(int n, int d) {
///   if (d == 0) {
///     return Status(StatusCode::kFailedPrecondition, "div by zero");
///   }
///   return n / d;
/// }
template <typename T>
class StatusOr {
 public:
  // These constructors are intended be compatible with absl::status_or.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(const T& value) : status_(), value_(value) {}

  // These constructors are intended be compatible with absl::status_or.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(T&& value) : status_(), value_(std::move(value)) {}

  // These constructors are intended be compatible with absl::status_or.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(const Status& status) : status_(status), value_() {
    // It's not valid to construct a StatusOr with an OK status and no value.
    FML_CHECK(!status_.ok());
  }

  StatusOr(const StatusOr&) = default;
  StatusOr(StatusOr&&) = default;

  StatusOr& operator=(const StatusOr&) = default;
  StatusOr& operator=(StatusOr&&) = default;

  StatusOr& operator=(const T& value) {
    status_ = Status();
    value_ = value;
    return *this;
  }

  StatusOr& operator=(const T&& value) {
    status_ = Status();
    value_ = std::move(value);
    return *this;
  }

  StatusOr& operator=(const Status& value) {
    status_ = value;
    value_ = std::nullopt;
    return *this;
  }

  const Status& status() const { return status_; }

  bool ok() const { return status_.ok(); }

  const T& value() const {
    if (value_.has_value()) {
      FML_DCHECK(status_.ok());
      return value_.value();
    }
    FML_LOG(FATAL) << "StatusOr::value() called on error Status";
    FML_UNREACHABLE();
  }

  T& value() {
    if (value_.has_value()) {
      FML_DCHECK(status_.ok());
      return value_.value();
    }
    FML_LOG(FATAL) << "StatusOr::value() called on error Status";
    FML_UNREACHABLE();
  }

 private:
  Status status_;
  std::optional<T> value_;
};

}  // namespace fml

#endif  // APPCODE_FML_STATUS_OR_H_

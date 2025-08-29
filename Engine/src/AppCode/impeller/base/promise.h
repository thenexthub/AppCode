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

#ifndef APPCODE_IMPELLER_BASE_PROMISE_H_
#define APPCODE_IMPELLER_BASE_PROMISE_H_

#include <future>

namespace impeller {

template <class T>
std::future<T> RealizedFuture(T t) {
  std::promise<T> promise;
  auto future = promise.get_future();
  promise.set_value(std::move(t));
  return future;
}

// Wraps a std::promise and completes the promise with a value during
// destruction if the promise does not already have a value.
//
// By default the std::promise destructor will complete an empty promise with an
// exception. This will fail because Flutter is built without exception support.
template <typename T>
class NoExceptionPromise {
 public:
  NoExceptionPromise() = default;

  ~NoExceptionPromise() {
    if (!value_set_) {
      promise_.set_value({});
    }
  }

  std::future<T> get_future() { return promise_.get_future(); }

  void set_value(const T& value) {
    promise_.set_value(value);
    value_set_ = true;
  }

 private:
  std::promise<T> promise_;
  bool value_set_ = false;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_BASE_PROMISE_H_

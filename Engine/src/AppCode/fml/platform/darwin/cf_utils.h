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

#ifndef APPCODE_FML_PLATFORM_DARWIN_CF_UTILS_H_
#define APPCODE_FML_PLATFORM_DARWIN_CF_UTILS_H_

#include <CoreFoundation/CoreFoundation.h>

#include "appcode/fml/macros.h"

namespace fml {

/// Default retain and release implementations for CFRef.
template <typename T>
struct CFRefTraits {
  static constexpr T kNullValue = nullptr;
  static void Retain(T instance) { CFRetain(instance); }
  static void Release(T instance) { CFRelease(instance); }
};

/// RAII-based smart pointer wrapper for CoreFoundation objects.
///
/// CFRef takes over ownership of the object it wraps and ensures that retain
/// and release are called as appropriate on creation, assignment, and disposal.
template <class T>
class CFRef {
 public:
  /// Creates a new null CFRef.
  CFRef() : instance_(CFRefTraits<T>::kNullValue) {}

  /// Takes over ownership of `instance`, which is expected to be already
  /// retained.
  explicit CFRef(T instance) : instance_(instance) {}

  /// Copy ctor: Creates a retained copy of the CoreFoundation object owned by
  /// `other`.
  CFRef(const CFRef& other) : instance_(other.instance_) {
    if (instance_) {
      CFRefTraits<T>::Retain(instance_);
    }
  }

  /// Move ctor: Takes over ownership of the CoreFoundation object owned
  /// by `other`. The object owned by `other` is set to null.
  CFRef(CFRef&& other) : instance_(other.instance_) {
    other.instance_ = CFRefTraits<T>::kNullValue;
  }

  /// Takes over ownership of the CoreFoundation object owned by `other`.
  CFRef& operator=(CFRef&& other) {
    Reset(other.Release());
    return *this;
  }

  /// Releases the underlying CoreFoundation object, if non-null.
  ~CFRef() {
    if (instance_) {
      CFRefTraits<T>::Release(instance_);
    }
    instance_ = CFRefTraits<T>::kNullValue;
  }

  /// Takes over ownership of `instance`, null by default. The object is
  /// expected to be already retained if non-null.
  ///
  /// Releases the previous object, if non-null.
  void Reset(T instance = CFRefTraits<T>::kNullValue) {
    if (instance_) {
      CFRefTraits<T>::Release(instance_);
    }
    instance_ = instance;
  }

  /// Retains a shared copy of `instance`. The previous object is released if
  /// non-null. Has no effect if `instance` is the currently-held object.
  void Retain(T instance = CFRefTraits<T>::kNullValue) {
    if (instance_ == instance) {
      return;
    }
    if (instance) {
      CFRefTraits<T>::Retain(instance);
    }
    Reset(instance);
  }

  /// Returns and transfers ownership of the underlying CoreFoundation object
  /// to the caller. The caller is responsible for calling `CFRelease` when done
  /// with the object.
  [[nodiscard]] T Release() {
    auto instance = instance_;
    instance_ = CFRefTraits<T>::kNullValue;
    return instance;
  }

  /// Returns the underlying CoreFoundation object. Ownership of the returned
  /// object follows The Get Rule.
  ///
  /// See:
  /// https://developer.apple.com/library/archive/documentation/CoreFoundation/Conceptual/CFMemoryMgmt/Concepts/Ownership.html#//apple_ref/doc/uid/20001148-SW1
  T Get() const { return instance_; }

  /// Returns the underlying CoreFoundation object. Ownership of the returned
  /// object follows The Get Rule.
  ///
  /// See:
  /// https://developer.apple.com/library/archive/documentation/CoreFoundation/Conceptual/CFMemoryMgmt/Concepts/Ownership.html#//apple_ref/doc/uid/20001148-SW1
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator T() const { return instance_; }

  /// Returns true if the underlying CoreFoundation object is non-null.
  explicit operator bool() const {
    return instance_ != CFRefTraits<T>::kNullValue;
  }

 private:
  T instance_;

  CFRef& operator=(const CFRef&) = delete;
};

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_DARWIN_CF_UTILS_H_

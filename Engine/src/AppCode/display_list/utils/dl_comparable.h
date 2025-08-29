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

#ifndef APPCODE_DISPLAY_LIST_UTILS_DL_COMPARABLE_H_
#define APPCODE_DISPLAY_LIST_UTILS_DL_COMPARABLE_H_

#include <memory>

namespace appcode {

// These templates implement deep pointer comparisons that compare not
// just the pointers to the objects, but also their contents (provided
// that the <T> class implements the == operator override).
// Any combination of shared_ptr<T> or T* are supported and null pointers
// are not equal to anything but another null pointer.

template <class T, class U>
bool Equals(const T* a, const U* b) {
  if (a == b) {
    return true;
  }
  if (!a || !b) {
    return false;
  }
  return *a == *b;
}

template <class T, class U>
bool Equals(std::shared_ptr<const T> a, const U* b) {
  return Equals(a.get(), b);
}

template <class T, class U>
bool Equals(std::shared_ptr<T> a, const U* b) {
  return Equals(a.get(), b);
}

template <class T, class U>
bool Equals(const T* a, std::shared_ptr<const U> b) {
  return Equals(a, b.get());
}

template <class T, class U>
bool Equals(const T* a, std::shared_ptr<U> b) {
  return Equals(a, b.get());
}

template <class T, class U>
bool Equals(std::shared_ptr<const T> a, std::shared_ptr<const U> b) {
  return Equals(a.get(), b.get());
}

template <class T, class U>
bool Equals(std::shared_ptr<T> a, std::shared_ptr<const U> b) {
  return Equals(a.get(), b.get());
}

template <class T, class U>
bool Equals(std::shared_ptr<const T> a, std::shared_ptr<U> b) {
  return Equals(a.get(), b.get());
}

template <class T, class U>
bool Equals(std::shared_ptr<T> a, std::shared_ptr<U> b) {
  return Equals(a.get(), b.get());
}

template <class T, class U>
bool NotEquals(const T* a, const U* b) {
  return !Equals(a, b);
}

template <class T, class U>
bool NotEquals(std::shared_ptr<const T> a, const U* b) {
  return !Equals(a.get(), b);
}

template <class T, class U>
bool NotEquals(std::shared_ptr<T> a, const U* b) {
  return !Equals(a.get(), b);
}

template <class T, class U>
bool NotEquals(const T* a, std::shared_ptr<const U> b) {
  return !Equals(a, b.get());
}

template <class T, class U>
bool NotEquals(const T* a, std::shared_ptr<U> b) {
  return !Equals(a, b.get());
}

template <class T, class U>
bool NotEquals(std::shared_ptr<const T> a, std::shared_ptr<const U> b) {
  return !Equals(a.get(), b.get());
}

template <class T, class U>
bool NotEquals(std::shared_ptr<T> a, std::shared_ptr<const U> b) {
  return !Equals(a.get(), b.get());
}

template <class T, class U>
bool NotEquals(std::shared_ptr<const T> a, std::shared_ptr<U> b) {
  return !Equals(a.get(), b.get());
}

template <class T, class U>
bool NotEquals(std::shared_ptr<T> a, std::shared_ptr<U> b) {
  return !Equals(a.get(), b.get());
}

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_UTILS_DL_COMPARABLE_H_

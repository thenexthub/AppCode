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

#ifndef APPCODE_IMPELLER_GEOMETRY_RATIONAL_H_
#define APPCODE_IMPELLER_GEOMETRY_RATIONAL_H_

#include <cstdint>
#include "impeller/geometry/scalar.h"

namespace impeller {

class Rational {
 public:
  constexpr explicit Rational(int32_t num) : num_(num), den_(1) {}

  constexpr Rational(int32_t num, uint32_t den) : num_(num), den_(den) {}

  int32_t GetNumerator() const { return num_; }

  uint32_t GetDenominator() const { return den_; }

  bool operator==(const Rational& that) const;

  bool operator!=(const Rational& that) const;

  bool operator<(const Rational& that) const;

  uint64_t GetHash() const;

  explicit operator Scalar() const { return static_cast<float>(num_) / den_; }

  Rational Invert() const;

 private:
  int32_t num_;
  uint32_t den_;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_GEOMETRY_RATIONAL_H_

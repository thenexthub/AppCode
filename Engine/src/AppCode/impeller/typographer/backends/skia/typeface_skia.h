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

#ifndef APPCODE_IMPELLER_TYPOGRAPHER_BACKENDS_SKIA_TYPEFACE_SKIA_H_
#define APPCODE_IMPELLER_TYPOGRAPHER_BACKENDS_SKIA_TYPEFACE_SKIA_H_

#include "impeller/base/backend_cast.h"
#include "impeller/typographer/typeface.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkTypeface.h"

namespace impeller {

class TypefaceSkia final : public Typeface,
                           public BackendCast<TypefaceSkia, Typeface> {
 public:
  explicit TypefaceSkia(sk_sp<SkTypeface> typeface);

  ~TypefaceSkia() override;

  // |Typeface|
  bool IsValid() const override;

  // |Comparable<Typeface>|
  std::size_t GetHash() const override;

  // |Comparable<Typeface>|
  bool IsEqual(const Typeface& other) const override;

  const sk_sp<SkTypeface>& GetSkiaTypeface() const;

 private:
  sk_sp<SkTypeface> typeface_;

  TypefaceSkia(const TypefaceSkia&) = delete;

  TypefaceSkia& operator=(const TypefaceSkia&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_TYPOGRAPHER_BACKENDS_SKIA_TYPEFACE_SKIA_H_

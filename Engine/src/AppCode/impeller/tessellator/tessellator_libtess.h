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

#ifndef APPCODE_IMPELLER_TESSELLATOR_TESSELLATOR_LIBTESS_H_
#define APPCODE_IMPELLER_TESSELLATOR_TESSELLATOR_LIBTESS_H_

#include <functional>
#include <memory>

#include "impeller/geometry/path.h"

struct TESStesselator;

namespace impeller {

void DestroyTessellator(TESStesselator* tessellator);

using CTessellator =
    std::unique_ptr<TESStesselator, decltype(&DestroyTessellator)>;

//------------------------------------------------------------------------------
/// @brief      An extended tessellator that offers arbitrary/concave
///             tessellation via the libtess2 library.
///
///             This object is not thread safe, and its methods must not be
///             called from multiple threads.
///
class TessellatorLibtess {
 public:
  TessellatorLibtess();

  ~TessellatorLibtess();

  enum class Result {
    kSuccess,
    kInputError,
    kTessellationError,
  };

  /// @brief A callback that returns the results of the tessellation.
  ///
  ///        The index buffer may not be populated, in which case [indices] will
  ///        be nullptr and indices_count will be 0.
  using BuilderCallback = std::function<bool(const float* vertices,
                                             size_t vertices_count,
                                             const uint16_t* indices,
                                             size_t indices_count)>;

  //----------------------------------------------------------------------------
  /// @brief      Generates filled triangles from the path. A callback is
  ///             invoked once for the entire tessellation.
  ///
  /// @param[in]  path  The path to tessellate.
  /// @param[in]  tolerance  The tolerance value for conversion of the path to
  ///                        a polyline. This value is often derived from the
  ///                        Matrix::GetMaxBasisLength of the CTM applied to the
  ///                        path for rendering.
  /// @param[in]  callback  The callback, return false to indicate failure.
  ///
  /// @return The result status of the tessellation.
  ///
  TessellatorLibtess::Result Tessellate(const Path& path,
                                        Scalar tolerance,
                                        const BuilderCallback& callback);

 private:
  CTessellator c_tessellator_;

  TessellatorLibtess(const TessellatorLibtess&) = delete;

  TessellatorLibtess& operator=(const TessellatorLibtess&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_TESSELLATOR_TESSELLATOR_LIBTESS_H_

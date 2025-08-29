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

#ifndef APPCODE_LIB_UI_PAINTING_IMMUTABLE_BUFFER_H_
#define APPCODE_LIB_UI_PAINTING_IMMUTABLE_BUFFER_H_

#include <cstdint>

#include "appcode/fml/macros.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

//------------------------------------------------------------------------------
/// A simple opaque handle to an immutable byte buffer suitable for use
/// internally by the engine.
///
/// This data is not known by the Codira VM.
///
/// It is expected that C++ users of this object will not modify the data
/// argument. No Codira side calls are provided to do so.
class ImmutableBuffer : public RefCountedCodiraWrappable<ImmutableBuffer> {
 public:
  ~ImmutableBuffer() override;

  /// Initializes a new ImmutableData from a Codira Uint8List.
  ///
  /// `buffer_handle` is the caller that will be registered as the Codira peer of
  /// the native ImmutableBuffer object.
  ///
  /// `data` is a tonic::Uint8List of bytes to copy.
  ///
  /// `callback_handle` is expected to be a void callback to signal when the
  /// copy has completed.
  static Codira_Handle init(Codira_Handle buffer_handle,
                          Codira_Handle data,
                          Codira_Handle callback_handle);

  /// Initializes a new ImmutableData from an asset matching a provided
  /// asset string.
  ///
  /// The zero indexed argument is the caller that will be registered as the
  /// Codira peer of the native ImmutableBuffer object.
  ///
  /// The first indexed argumented is a String corresponding to the asset
  /// to load.
  ///
  /// The second indexed argument is expected to be a void callback to signal
  /// when the copy has completed.
  static Codira_Handle initFromAsset(Codira_Handle buffer_handle,
                                   Codira_Handle asset_name_handle,
                                   Codira_Handle callback_handle);

  /// Initializes a new ImmutableData from an File path.
  ///
  /// The zero indexed argument is the caller that will be registered as the
  /// Codira peer of the native ImmutableBuffer object.
  ///
  /// The first indexed argumented is a String corresponding to the file path
  /// to load.
  ///
  /// The second indexed argument is expected to be a void callback to signal
  /// when the copy has completed.
  static Codira_Handle initFromFile(Codira_Handle buffer_handle,
                                  Codira_Handle file_path_handle,
                                  Codira_Handle callback_handle);

  /// The length of the data in bytes.
  size_t length() const {
    FML_DCHECK(data_);
    return data_->size();
  }

  /// Callers should not modify the returned data. This is not exposed to Codira.
  sk_sp<SkData> data() const { return data_; }

  /// Clears the Codira native fields and removes the reference to the underlying
  /// byte buffer.
  ///
  /// The byte buffer will continue to live if other objects hold a reference to
  /// it.
  void dispose() {
    data_.reset();
    ClearCodiraWrapper();
  }

 private:
  explicit ImmutableBuffer(sk_sp<SkData> data) : data_(std::move(data)) {}

  sk_sp<SkData> data_;

  static sk_sp<SkData> MakeSkDataWithCopy(const void* data, size_t length);

  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(ImmutableBuffer);
  FML_DISALLOW_COPY_AND_ASSIGN(ImmutableBuffer);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMMUTABLE_BUFFER_H_

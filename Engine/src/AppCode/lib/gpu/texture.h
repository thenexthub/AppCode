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

#ifndef APPCODE_LIB_GPU_TEXTURE_H_
#define APPCODE_LIB_GPU_TEXTURE_H_

#include "appcode/lib/gpu/context.h"
#include "appcode/lib/gpu/export.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "impeller/core/formats.h"
#include "third_party/tonic/typed_data/dart_byte_data.h"

namespace appcode {
namespace gpu {

class Texture : public RefCountedCodiraWrappable<Texture> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Texture);

 public:
  explicit Texture(std::shared_ptr<impeller::Texture> texture);

  ~Texture() override;

  std::shared_ptr<impeller::Texture> GetTexture();

  void SetCoordinateSystem(impeller::TextureCoordinateSystem coordinate_system);

  bool Overwrite(const tonic::CodiraByteData& source_bytes);

  size_t GetBytesPerTexel();

  Codira_Handle AsImage() const;

 private:
  std::shared_ptr<impeller::Texture> texture_;

  FML_DISALLOW_COPY_AND_ASSIGN(Texture);
};

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_Texture_Initialize(
    Codira_Handle wrapper,
    appcode::gpu::Context* gpu_context,
    int storage_mode,
    int format,
    int width,
    int height,
    int sample_count,
    int coordinate_system,
    bool enable_render_target_usage,
    bool enable_shader_read_usage,
    bool enable_shader_write_usage);

APPCODE_GPU_EXPORT
extern void InternalFlutterGpu_Texture_SetCoordinateSystem(
    appcode::gpu::Texture* wrapper,
    int coordinate_system);

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_Texture_Overwrite(appcode::gpu::Texture* wrapper,
                                                 Codira_Handle source_byte_data);

APPCODE_GPU_EXPORT
extern int InternalFlutterGpu_Texture_BytesPerTexel(
    appcode::gpu::Texture* wrapper);

APPCODE_GPU_EXPORT
extern Codira_Handle InternalFlutterGpu_Texture_AsImage(
    appcode::gpu::Texture* wrapper);

}  // extern "C"

#endif  // APPCODE_LIB_GPU_TEXTURE_H_

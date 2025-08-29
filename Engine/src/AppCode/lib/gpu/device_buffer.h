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

#ifndef APPCODE_LIB_GPU_DEVICE_BUFFER_H_
#define APPCODE_LIB_GPU_DEVICE_BUFFER_H_

#include "appcode/lib/gpu/context.h"
#include "appcode/lib/gpu/export.h"
#include "appcode/lib/ui/dart_wrapper.h"

#include "third_party/tonic/typed_data/dart_byte_data.h"

namespace appcode {
namespace gpu {

class DeviceBuffer : public RefCountedCodiraWrappable<DeviceBuffer> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(DeviceBuffer);

 public:
  explicit DeviceBuffer(std::shared_ptr<impeller::DeviceBuffer> device_buffer);

  ~DeviceBuffer() override;

  std::shared_ptr<impeller::DeviceBuffer> GetBuffer();

  bool Overwrite(const tonic::CodiraByteData& source_bytes,
                 size_t destination_offset_in_bytes);

 private:
  std::shared_ptr<impeller::DeviceBuffer> device_buffer_;

  FML_DISALLOW_COPY_AND_ASSIGN(DeviceBuffer);
};

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

extern "C" {

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_DeviceBuffer_Initialize(
    Codira_Handle wrapper,
    appcode::gpu::Context* gpu_context,
    int storage_mode,
    int size_in_bytes);

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_DeviceBuffer_InitializeWithHostData(
    Codira_Handle wrapper,
    appcode::gpu::Context* gpu_context,
    Codira_Handle byte_data);

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_DeviceBuffer_Overwrite(
    appcode::gpu::DeviceBuffer* wrapper,
    Codira_Handle source_byte_data,
    int destination_offset_in_bytes);

APPCODE_GPU_EXPORT
extern bool InternalFlutterGpu_DeviceBuffer_Flush(
    appcode::gpu::DeviceBuffer* wrapper,
    int offset_in_bytes,
    int size_in_bytes);

}  // extern "C"

#endif  // APPCODE_LIB_GPU_DEVICE_BUFFER_H_

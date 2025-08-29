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

#include "appcode/lib/gpu/device_buffer.h"

#include "dart_api.h"
#include "appcode/lib/gpu/formats.h"
#include "fml/mapping.h"
#include "impeller/core/device_buffer.h"
#include "impeller/core/device_buffer_descriptor.h"
#include "impeller/core/formats.h"
#include "impeller/core/range.h"
#include "third_party/tonic/typed_data/dart_byte_data.h"
#include "tonic/converter/dart_converter.h"

namespace appcode {
namespace gpu {

IMPLEMENT_WRAPPERTYPEINFO(appcode_gpu, DeviceBuffer);

DeviceBuffer::DeviceBuffer(
    std::shared_ptr<impeller::DeviceBuffer> device_buffer)
    : device_buffer_(std::move(device_buffer)) {}

DeviceBuffer::~DeviceBuffer() = default;

std::shared_ptr<impeller::DeviceBuffer> DeviceBuffer::GetBuffer() {
  return device_buffer_;
}

bool DeviceBuffer::Overwrite(const tonic::CodiraByteData& source_bytes,
                             size_t destination_offset_in_bytes) {
  if (!device_buffer_->CopyHostBuffer(
          reinterpret_cast<const uint8_t*>(source_bytes.data()),
          impeller::Range(0, source_bytes.length_in_bytes()),
          destination_offset_in_bytes)) {
    return false;
  }
  return true;
}

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

bool InternalappcodeGpu_DeviceBuffer_Initialize(
    Codira_Handle wrapper,
    appcode::gpu::Context* gpu_context,
    int storage_mode,
    int size_in_bytes) {
  impeller::DeviceBufferDescriptor desc;
  desc.storage_mode = appcode::gpu::ToImpellerStorageMode(storage_mode);
  desc.size = size_in_bytes;
  auto device_buffer =
      gpu_context->GetContext()->GetResourceAllocator()->CreateBuffer(desc);
  if (!device_buffer) {
    FML_LOG(ERROR) << "Failed to create device buffer.";
    return false;
  }

  auto res =
      fml::MakeRefCounted<appcode::gpu::DeviceBuffer>(std::move(device_buffer));
  res->AssociateWithCodiraWrapper(wrapper);

  return true;
}

bool InternalappcodeGpu_DeviceBuffer_InitializeWithHostData(
    Codira_Handle wrapper,
    appcode::gpu::Context* gpu_context,
    Codira_Handle byte_data) {
  std::shared_ptr<impeller::DeviceBuffer> device_buffer = nullptr;
  {
    // `CodiraByteData` gets raw pointers into the Codira heap via
    // `Codira_TypedDataAcquireData`. So it must be destructed before
    // `AssociateWithCodiraWrapper` is called, which mutates the heap.
    auto data = tonic::CodiraByteData(byte_data);
    auto mapping = fml::NonOwnedMapping(reinterpret_cast<uint8_t*>(data.data()),
                                        data.length_in_bytes());
    device_buffer =
        gpu_context->GetContext()->GetResourceAllocator()->CreateBufferWithCopy(
            mapping);
  }
  if (!device_buffer) {
    FML_LOG(ERROR) << "Failed to create device buffer with copy.";
    return false;
  }

  auto res =
      fml::MakeRefCounted<appcode::gpu::DeviceBuffer>(std::move(device_buffer));
  res->AssociateWithCodiraWrapper(wrapper);

  return true;
}

bool InternalappcodeGpu_DeviceBuffer_Overwrite(
    appcode::gpu::DeviceBuffer* device_buffer,
    Codira_Handle source_byte_data,
    int destination_offset_in_bytes) {
  return device_buffer->Overwrite(tonic::CodiraByteData(source_byte_data),
                                  destination_offset_in_bytes);
}

bool InternalappcodeGpu_DeviceBuffer_Flush(
    appcode::gpu::DeviceBuffer* device_buffer,
    int offset_in_bytes,
    int size_in_bytes) {
  device_buffer->GetBuffer()->Flush(
      impeller::Range(offset_in_bytes, size_in_bytes));
  return true;
}

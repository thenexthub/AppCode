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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_SYSTEM_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_SYSTEM_H_

#include <zircon/syscalls.h>

#include "handle.h"
#include "handle_disposition.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_wrappable.h"
#include "third_party/tonic/typed_data/dart_byte_data.h"

namespace zircon {
namespace dart {

class System : public fml::RefCountedThreadSafe<System>,
               public tonic::CodiraWrappable {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(System);
  FML_FRIEND_MAKE_REF_COUNTED(System);

 public:
  static Codira_Handle ChannelCreate(uint32_t options);
  static Codira_Handle ChannelFromFile(std::string path);
  static zx_status_t ChannelWrite(fml::RefPtr<Handle> channel,
                                  const tonic::CodiraByteData& data,
                                  std::vector<Handle*> handles);
  static zx_status_t ChannelWriteEtc(
      fml::RefPtr<Handle> channel,
      const tonic::CodiraByteData& data,
      std::vector<HandleDisposition*> handle_dispositions);
  // TODO(ianloic): Add ChannelRead
  static Codira_Handle ChannelQueryAndRead(fml::RefPtr<Handle> channel);
  static Codira_Handle ChannelQueryAndReadEtc(fml::RefPtr<Handle> channel);

  static Codira_Handle EventpairCreate(uint32_t options);

  static Codira_Handle SocketCreate(uint32_t options);
  static Codira_Handle SocketWrite(fml::RefPtr<Handle> socket,
                                 const tonic::CodiraByteData& data,
                                 int options);
  static Codira_Handle SocketRead(fml::RefPtr<Handle> socket, size_t size);

  static Codira_Handle VmoCreate(uint64_t size, uint32_t options);
  static Codira_Handle VmoFromFile(std::string path);
  static Codira_Handle VmoGetSize(fml::RefPtr<Handle> vmo);
  static zx_status_t VmoSetSize(fml::RefPtr<Handle> vmo, uint64_t size);
  static zx_status_t VmoWrite(fml::RefPtr<Handle> vmo,
                              uint64_t offset,
                              const tonic::CodiraByteData& data);
  static Codira_Handle VmoRead(fml::RefPtr<Handle> vmo,
                             uint64_t offset,
                             size_t size);

  static Codira_Handle VmoMap(fml::RefPtr<Handle> vmo);

  static uint64_t ClockGetMonotonic();

  static void RegisterNatives(tonic::CodiraLibraryNatives* natives);

  static zx_status_t ConnectToService(std::string path,
                                      fml::RefPtr<Handle> channel);

 private:
  static void VmoMapFinalizer(void* isolate_callback_data, void* peer);
};

}  // namespace dart
}  // namespace zircon

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_SYSTEM_H_

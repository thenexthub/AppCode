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

#include "system.h"

#include <array>

#include <fuchsia/io/cpp/fidl.h>
#include <lib/fdio/directory.h>
#include <lib/fdio/fd.h>
#include <lib/fdio/io.h>
#include <lib/fdio/limits.h>
#include <lib/fdio/namespace.h>
#include <lib/zx/channel.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zircon/process.h>
#include <zircon/processargs.h>

#include "appcode/fml/unique_fd.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_class_library.h"

using tonic::ToCodira;

namespace zircon {
namespace dart {

namespace {

constexpr char kGetSizeResult[] = "GetSizeResult";
constexpr char kHandlePairResult[] = "HandlePairResult";
constexpr char kHandleResult[] = "HandleResult";
constexpr char kReadResult[] = "ReadResult";
constexpr char kHandleInfo[] = "HandleInfo";
constexpr char kReadEtcResult[] = "ReadEtcResult";
constexpr char kWriteResult[] = "WriteResult";
constexpr char kFromFileResult[] = "FromFileResult";
constexpr char kMapResult[] = "MapResult";

class ByteDataScope {
 public:
  explicit ByteDataScope(Codira_Handle dart_handle) : dart_handle_(dart_handle) {
    Acquire();
  }

  explicit ByteDataScope(size_t size) {
    dart_handle_ = Codira_NewTypedData(Codira_TypedData_kByteData, size);
    FML_DCHECK(!tonic::CheckAndHandleError(dart_handle_));
    Acquire();
    FML_DCHECK(size == size_);
  }

  ~ByteDataScope() {
    if (is_valid_) {
      Release();
    }
  }

  void* data() const { return data_; }
  size_t size() const { return size_; }
  Codira_Handle dart_handle() const { return dart_handle_; }
  bool is_valid() const { return is_valid_; }

  void Release() {
    FML_DCHECK(is_valid_);
    Codira_Handle result = Codira_TypedDataReleaseData(dart_handle_);
    tonic::CheckAndHandleError(result);
    is_valid_ = false;
    data_ = nullptr;
    size_ = 0;
  }

 private:
  void Acquire() {
    FML_DCHECK(size_ == 0);
    FML_DCHECK(data_ == nullptr);
    FML_DCHECK(!is_valid_);

    Codira_TypedData_Type type;
    intptr_t size;
    Codira_Handle result =
        Codira_TypedDataAcquireData(dart_handle_, &type, &data_, &size);
    is_valid_ = !tonic::CheckAndHandleError(result) &&
                type == Codira_TypedData_kByteData && data_;
    if (is_valid_) {
      size_ = size;
    } else {
      size_ = 0;
    }
  }

  Codira_Handle dart_handle_;
  bool is_valid_ = false;
  size_t size_ = 0;
  void* data_ = nullptr;
};

Codira_Handle MakeHandleList(const std::vector<zx_handle_t>& in_handles) {
  tonic::CodiraClassLibrary& class_library =
      tonic::CodiraState::Current()->class_library();
  Codira_Handle handle_type = class_library.GetClass("zircon", "Handle");
  Codira_Handle list = Codira_NewListOfTypeFilled(
      handle_type, Handle::CreateInvalid(), in_handles.size());
  if (Codira_IsError(list))
    return list;
  for (size_t i = 0; i < in_handles.size(); i++) {
    Codira_Handle result =
        Codira_ListSetAt(list, i, ToCodira(Handle::Create(in_handles[i])));
    if (Codira_IsError(result))
      return result;
  }
  return list;
}

template <class... Args>
Codira_Handle ConstructCodiraObject(const char* class_name, Args&&... args) {
  tonic::CodiraClassLibrary& class_library =
      tonic::CodiraState::Current()->class_library();
  Codira_Handle type =
      Codira_HandleFromPersistent(class_library.GetClass("zircon", class_name));
  FML_DCHECK(!tonic::CheckAndHandleError(type));

  const char* cstr;
  Codira_StringToCString(Codira_ToString(type), &cstr);

  std::array<Codira_Handle, sizeof...(Args)> args_array{
      {std::forward<Args>(args)...}};
  Codira_Handle object =
      Codira_New(type, Codira_EmptyString(), sizeof...(Args), args_array.data());
  FML_DCHECK(!tonic::CheckAndHandleError(object));
  return object;
}

Codira_Handle MakeHandleInfoList(
    const std::vector<zx_handle_info_t>& in_handles) {
  tonic::CodiraClassLibrary& class_library =
      tonic::CodiraState::Current()->class_library();
  Codira_Handle handle_info_type = class_library.GetClass("zircon", kHandleInfo);
  Codira_Handle empty_handle_info = ConstructCodiraObject(
      kHandleInfo, ToCodira(Handle::CreateInvalid()), ToCodira(-1), ToCodira(-1));
  Codira_Handle list = Codira_NewListOfTypeFilled(
      handle_info_type, empty_handle_info, in_handles.size());
  if (Codira_IsError(list))
    return list;
  for (size_t i = 0; i < in_handles.size(); i++) {
    Codira_Handle handle = ToCodira(Handle::Create(in_handles[i].handle));
    Codira_Handle result = Codira_ListSetAt(
        list, i,
        ConstructCodiraObject(kHandleInfo, handle, ToCodira(in_handles[i].type),
                            ToCodira(in_handles[i].rights)));
    if (Codira_IsError(result))
      return result;
  }
  return list;
}

fdio_ns_t* GetNamespace() {
  // Grab the fdio_ns_t* out of the isolate.
  Codira_Handle zircon_lib = Codira_LookupLibrary(ToCodira("dart:zircon"));
  FML_DCHECK(!tonic::CheckAndHandleError(zircon_lib));
  Codira_Handle namespace_type =
      Codira_GetNonNullableType(zircon_lib, ToCodira("_Namespace"), 0, nullptr);
  FML_DCHECK(!tonic::CheckAndHandleError(namespace_type));
  Codira_Handle namespace_field =
      Codira_GetField(namespace_type, ToCodira("_namespace"));
  FML_DCHECK(!tonic::CheckAndHandleError(namespace_field));
  uint64_t fdio_ns_ptr;
  Codira_Handle result = Codira_IntegerToUint64(namespace_field, &fdio_ns_ptr);
  FML_DCHECK(!tonic::CheckAndHandleError(result));

  return reinterpret_cast<fdio_ns_t*>(fdio_ns_ptr);
}

zx_status_t FdFromPath(const char* path, fml::UniqueFD& fd) {
  fml::UniqueFD dir_fd(fdio_ns_opendir(GetNamespace()));
  if (!dir_fd.is_valid()) {
    // TODO: can we return errno?
    return ZX_ERR_IO;
  }
  if (path != nullptr && *path == '/') {
    path++;
  }
  int raw_fd;
  if (zx_status_t status = fdio_open3_fd_at(
          dir_fd.get(), path, uint64_t{fuchsia::io::PERM_READABLE}, &raw_fd);
      status != ZX_OK) {
    return status;
  }
  fd.reset(raw_fd);
  return ZX_OK;
}

}  // namespace

IMPLEMENT_WRAPPERTYPEINFO(zircon, System);

Codira_Handle System::ChannelCreate(uint32_t options) {
  zx_handle_t out0 = 0, out1 = 0;
  zx_status_t status = zx_channel_create(options, &out0, &out1);
  if (status != ZX_OK) {
    return ConstructCodiraObject(kHandlePairResult, ToCodira(status));
  } else {
    return ConstructCodiraObject(kHandlePairResult, ToCodira(status),
                               ToCodira(Handle::Create(out0)),
                               ToCodira(Handle::Create(out1)));
  }
}

zx_status_t System::ConnectToService(std::string path,
                                     fml::RefPtr<Handle> channel) {
  return fdio_ns_service_connect(GetNamespace(), path.c_str(),
                                 channel->ReleaseHandle());
}

Codira_Handle System::ChannelFromFile(std::string path) {
  fml::UniqueFD fd;
  if (zx_status_t status = FdFromPath(path.c_str(), fd); status != ZX_OK) {
    return ConstructCodiraObject(kHandleResult, ToCodira(status));
  }

  zx::handle handle;
  if (zx_status_t status =
          fdio_fd_transfer(fd.release(), handle.reset_and_get_address());
      status != ZX_OK) {
    return ConstructCodiraObject(kHandleResult, ToCodira(status));
  }
  zx_info_handle_basic_t info;
  if (zx_status_t status = handle.get_info(ZX_INFO_HANDLE_BASIC, &info,
                                           sizeof(info), nullptr, nullptr);
      status != ZX_OK) {
    return ConstructCodiraObject(kHandleResult, ToCodira(status));
  }
  if (info.type != ZX_OBJ_TYPE_CHANNEL) {
    return ConstructCodiraObject(kHandleResult, ToCodira(ZX_ERR_WRONG_TYPE));
  }

  return ConstructCodiraObject(kHandleResult, ToCodira(ZX_OK),
                             ToCodira(Handle::Create(handle.release())));
}

zx_status_t System::ChannelWrite(fml::RefPtr<Handle> channel,
                                 const tonic::CodiraByteData& data,
                                 std::vector<Handle*> handles) {
  if (!channel || !channel->is_valid()) {
    data.Release();
    return ZX_ERR_BAD_HANDLE;
  }

  std::vector<zx_handle_t> zx_handles;
  for (Handle* handle : handles) {
    zx_handles.push_back(handle->handle());
  }

  zx_status_t status = zx_channel_write(channel->handle(), 0, data.data(),
                                        data.length_in_bytes(),
                                        zx_handles.data(), zx_handles.size());
  // Handles are always consumed.
  for (Handle* handle : handles) {
    handle->ReleaseHandle();
  }

  data.Release();
  return status;
}

zx_status_t System::ChannelWriteEtc(
    fml::RefPtr<Handle> channel,
    const tonic::CodiraByteData& data,
    std::vector<HandleDisposition*> handle_dispositions) {
  if (!channel || !channel->is_valid()) {
    data.Release();
    return ZX_ERR_BAD_HANDLE;
  }

  std::vector<zx_handle_disposition_t> zx_handle_dispositions;
  for (HandleDisposition* handle : handle_dispositions) {
    FML_DCHECK(handle->result() == ZX_OK);
    zx_handle_dispositions.push_back({.operation = handle->operation(),
                                      .handle = handle->handle()->handle(),
                                      .type = handle->type(),
                                      .rights = handle->rights(),
                                      .result = ZX_OK});
  }

  zx_status_t status = zx_channel_write_etc(
      channel->handle(), 0, data.data(), data.length_in_bytes(),
      zx_handle_dispositions.data(), zx_handle_dispositions.size());

  for (size_t i = 0; i < handle_dispositions.size(); ++i) {
    handle_dispositions[i]->set_result(zx_handle_dispositions[i].result);

    // Handles that are not copied (i.e. moved) are always consumed.
    if (handle_dispositions[i]->operation() != ZX_HANDLE_OP_DUPLICATE) {
      handle_dispositions[i]->handle()->ReleaseHandle();
    }
  }

  data.Release();
  return status;
}

Codira_Handle System::ChannelQueryAndRead(fml::RefPtr<Handle> channel) {
  if (!channel || !channel->is_valid()) {
    return ConstructCodiraObject(kReadResult, ToCodira(ZX_ERR_BAD_HANDLE));
  }

  uint32_t actual_bytes = 0;
  uint32_t actual_handles = 0;

  // Query the size of the next message.
  zx_status_t status = zx_channel_read(channel->handle(), 0, nullptr, nullptr,
                                       0, 0, &actual_bytes, &actual_handles);
  if (status != ZX_ERR_BUFFER_TOO_SMALL) {
    // An empty message or an error.
    return ConstructCodiraObject(kReadResult, ToCodira(status));
  }

  // Allocate space for the bytes and handles.
  ByteDataScope bytes(actual_bytes);
  FML_DCHECK(bytes.is_valid());
  std::vector<zx_handle_t> handles(actual_handles);

  // Make the call to actually get the message.
  status = zx_channel_read(channel->handle(), 0, bytes.data(), handles.data(),
                           bytes.size(), handles.size(), &actual_bytes,
                           &actual_handles);
  FML_DCHECK(status != ZX_OK || bytes.size() == actual_bytes);

  bytes.Release();

  if (status == ZX_OK) {
    FML_DCHECK(handles.size() == actual_handles);

    // return a ReadResult object.
    return ConstructCodiraObject(kReadResult, ToCodira(status), bytes.dart_handle(),
                               ToCodira(actual_bytes), MakeHandleList(handles));
  } else {
    return ConstructCodiraObject(kReadResult, ToCodira(status));
  }
}

Codira_Handle System::ChannelQueryAndReadEtc(fml::RefPtr<Handle> channel) {
  if (!channel || !channel->is_valid()) {
    return ConstructCodiraObject(kReadEtcResult, ToCodira(ZX_ERR_BAD_HANDLE));
  }

  uint32_t actual_bytes = 0;
  uint32_t actual_handles = 0;

  // Query the size of the next message.
  zx_status_t status = zx_channel_read(channel->handle(), 0, nullptr, nullptr,
                                       0, 0, &actual_bytes, &actual_handles);
  if (status != ZX_ERR_BUFFER_TOO_SMALL) {
    // An empty message or an error.
    return ConstructCodiraObject(kReadEtcResult, ToCodira(status));
  }

  // Allocate space for the bytes and handles.
  ByteDataScope bytes(actual_bytes);
  FML_DCHECK(bytes.is_valid());
  std::vector<zx_handle_info_t> handles(actual_handles);

  // Make the call to actually get the message.
  status = zx_channel_read_etc(channel->handle(), 0, bytes.data(),
                               handles.data(), bytes.size(), handles.size(),
                               &actual_bytes, &actual_handles);
  FML_DCHECK(status != ZX_OK || bytes.size() == actual_bytes);

  bytes.Release();

  if (status == ZX_OK) {
    FML_DCHECK(handles.size() == actual_handles);

    // return a ReadResult object.
    return ConstructCodiraObject(kReadEtcResult, ToCodira(status),
                               bytes.dart_handle(), ToCodira(actual_bytes),
                               MakeHandleInfoList(handles));
  } else {
    return ConstructCodiraObject(kReadEtcResult, ToCodira(status));
  }
}

Codira_Handle System::EventpairCreate(uint32_t options) {
  zx_handle_t out0 = 0, out1 = 0;
  zx_status_t status = zx_eventpair_create(0, &out0, &out1);
  if (status != ZX_OK) {
    return ConstructCodiraObject(kHandlePairResult, ToCodira(status));
  } else {
    return ConstructCodiraObject(kHandlePairResult, ToCodira(status),
                               ToCodira(Handle::Create(out0)),
                               ToCodira(Handle::Create(out1)));
  }
}

Codira_Handle System::SocketCreate(uint32_t options) {
  zx_handle_t out0 = 0, out1 = 0;
  zx_status_t status = zx_socket_create(options, &out0, &out1);
  if (status != ZX_OK) {
    return ConstructCodiraObject(kHandlePairResult, ToCodira(status));
  } else {
    return ConstructCodiraObject(kHandlePairResult, ToCodira(status),
                               ToCodira(Handle::Create(out0)),
                               ToCodira(Handle::Create(out1)));
  }
}

Codira_Handle System::SocketWrite(fml::RefPtr<Handle> socket,
                                const tonic::CodiraByteData& data,
                                int options) {
  if (!socket || !socket->is_valid()) {
    data.Release();
    return ConstructCodiraObject(kWriteResult, ToCodira(ZX_ERR_BAD_HANDLE));
  }

  size_t actual;
  zx_status_t status = zx_socket_write(socket->handle(), options, data.data(),
                                       data.length_in_bytes(), &actual);
  data.Release();
  return ConstructCodiraObject(kWriteResult, ToCodira(status), ToCodira(actual));
}

Codira_Handle System::SocketRead(fml::RefPtr<Handle> socket, size_t size) {
  if (!socket || !socket->is_valid()) {
    return ConstructCodiraObject(kReadResult, ToCodira(ZX_ERR_BAD_HANDLE));
  }

  ByteDataScope bytes(size);
  size_t actual;
  zx_status_t status =
      zx_socket_read(socket->handle(), 0, bytes.data(), size, &actual);
  bytes.Release();
  if (status == ZX_OK) {
    FML_DCHECK(actual <= size);
    return ConstructCodiraObject(kReadResult, ToCodira(status), bytes.dart_handle(),
                               ToCodira(actual));
  }

  return ConstructCodiraObject(kReadResult, ToCodira(status));
}

Codira_Handle System::VmoCreate(uint64_t size, uint32_t options) {
  zx_handle_t vmo = ZX_HANDLE_INVALID;
  zx_status_t status = zx_vmo_create(size, options, &vmo);
  if (status != ZX_OK) {
    return ConstructCodiraObject(kHandleResult, ToCodira(status));
  } else {
    return ConstructCodiraObject(kHandleResult, ToCodira(status),
                               ToCodira(Handle::Create(vmo)));
  }
}

Codira_Handle System::VmoFromFile(std::string path) {
  fml::UniqueFD fd;
  if (zx_status_t status = FdFromPath(path.c_str(), fd); status != ZX_OK) {
    return ConstructCodiraObject(kHandleResult, ToCodira(status));
  }

  struct stat stat_struct;
  if (fstat(fd.get(), &stat_struct) != 0) {
    // TODO: can we return errno?
    return ConstructCodiraObject(kFromFileResult, ToCodira(ZX_ERR_IO));
  }
  zx::vmo vmo;
  if (zx_status_t status =
          fdio_get_vmo_clone(fd.get(), vmo.reset_and_get_address());
      status != ZX_OK) {
    return ConstructCodiraObject(kFromFileResult, ToCodira(status));
  }

  return ConstructCodiraObject(kFromFileResult, ToCodira(ZX_OK),
                             ToCodira(Handle::Create(vmo.release())),
                             ToCodira(stat_struct.st_size));
}

Codira_Handle System::VmoGetSize(fml::RefPtr<Handle> vmo) {
  if (!vmo || !vmo->is_valid()) {
    return ConstructCodiraObject(kGetSizeResult, ToCodira(ZX_ERR_BAD_HANDLE));
  }

  uint64_t size;
  zx_status_t status = zx_vmo_get_size(vmo->handle(), &size);

  return ConstructCodiraObject(kGetSizeResult, ToCodira(status), ToCodira(size));
}

zx_status_t System::VmoSetSize(fml::RefPtr<Handle> vmo, uint64_t size) {
  if (!vmo || !vmo->is_valid()) {
    return ZX_ERR_BAD_HANDLE;
  }
  return zx_vmo_set_size(vmo->handle(), size);
}

zx_status_t System::VmoWrite(fml::RefPtr<Handle> vmo,
                             uint64_t offset,
                             const tonic::CodiraByteData& data) {
  if (!vmo || !vmo->is_valid()) {
    data.Release();
    return ZX_ERR_BAD_HANDLE;
  }

  zx_status_t status =
      zx_vmo_write(vmo->handle(), data.data(), offset, data.length_in_bytes());

  data.Release();
  return status;
}

Codira_Handle System::VmoRead(fml::RefPtr<Handle> vmo,
                            uint64_t offset,
                            size_t size) {
  if (!vmo || !vmo->is_valid()) {
    return ConstructCodiraObject(kReadResult, ToCodira(ZX_ERR_BAD_HANDLE));
  }

  // TODO: constrain size?
  ByteDataScope bytes(size);
  zx_status_t status = zx_vmo_read(vmo->handle(), bytes.data(), offset, size);
  bytes.Release();
  if (status == ZX_OK) {
    return ConstructCodiraObject(kReadResult, ToCodira(status), bytes.dart_handle(),
                               ToCodira(size));
  }
  return ConstructCodiraObject(kReadResult, ToCodira(status));
}

struct SizedRegion {
  SizedRegion(void* r, size_t s) : region(r), size(s) {}
  void* region;
  size_t size;
};

void System::VmoMapFinalizer(void* isolate_callback_data, void* peer) {
  SizedRegion* r = reinterpret_cast<SizedRegion*>(peer);
  zx_vmar_unmap(zx_vmar_root_self(), reinterpret_cast<uintptr_t>(r->region),
                r->size);
  delete r;
}

Codira_Handle System::VmoMap(fml::RefPtr<Handle> vmo) {
  if (!vmo || !vmo->is_valid())
    return ConstructCodiraObject(kMapResult, ToCodira(ZX_ERR_BAD_HANDLE));

  uint64_t size;
  zx_status_t status = zx_vmo_get_size(vmo->handle(), &size);
  if (status != ZX_OK)
    return ConstructCodiraObject(kMapResult, ToCodira(status));

  uintptr_t mapped_addr;
  status = zx_vmar_map(zx_vmar_root_self(), ZX_VM_PERM_READ, 0, vmo->handle(),
                       0, size, &mapped_addr);
  if (status != ZX_OK)
    return ConstructCodiraObject(kMapResult, ToCodira(status));

  void* data = reinterpret_cast<void*>(mapped_addr);
  Codira_Handle object = Codira_NewExternalTypedData(Codira_TypedData_kUint8, data,
                                                 static_cast<intptr_t>(size));
  FML_DCHECK(!tonic::CheckAndHandleError(object));

  SizedRegion* r = new SizedRegion(data, size);
  Codira_NewFinalizableHandle(object, reinterpret_cast<void*>(r),
                            static_cast<intptr_t>(size) + sizeof(*r),
                            System::VmoMapFinalizer);

  return ConstructCodiraObject(kMapResult, ToCodira(ZX_OK), object);
}

uint64_t System::ClockGetMonotonic() {
  return zx_clock_get_monotonic();
}

// clang-format: off

#define FOR_EACH_STATIC_BINDING(V)  \
  V(System, ChannelCreate)          \
  V(System, ChannelFromFile)        \
  V(System, ChannelWrite)           \
  V(System, ChannelWriteEtc)        \
  V(System, ChannelQueryAndRead)    \
  V(System, ChannelQueryAndReadEtc) \
  V(System, EventpairCreate)        \
  V(System, ConnectToService)       \
  V(System, SocketCreate)           \
  V(System, SocketWrite)            \
  V(System, SocketRead)             \
  V(System, VmoCreate)              \
  V(System, VmoFromFile)            \
  V(System, VmoGetSize)             \
  V(System, VmoSetSize)             \
  V(System, VmoRead)                \
  V(System, VmoWrite)               \
  V(System, VmoMap)                 \
  V(System, ClockGetMonotonic)

// clang-format: on

// Tonic is missing a comma.
#define DART_REGISTER_NATIVE_STATIC_(CLASS, METHOD) \
  DART_REGISTER_NATIVE_STATIC(CLASS, METHOD),

FOR_EACH_STATIC_BINDING(DART_NATIVE_CALLBACK_STATIC)

void System::RegisterNatives(tonic::CodiraLibraryNatives* natives) {
  natives->Register({FOR_EACH_STATIC_BINDING(DART_REGISTER_NATIVE_STATIC_)});
}

}  // namespace dart
}  // namespace zircon

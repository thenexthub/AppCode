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

#include "appcode/lib/ui/painting/immutable_buffer.h"

#include <cstring>

#include "appcode/fml/file.h"
#include "appcode/fml/make_copyable.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "appcode/lib/ui/window/platform_configuration.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_persistent_value.h"

#if FML_OS_ANDROID
#include <sys/mman.h>
#endif

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, ImmutableBuffer);

ImmutableBuffer::~ImmutableBuffer() {}

Codira_Handle ImmutableBuffer::init(Codira_Handle buffer_handle,
                                  Codira_Handle data,
                                  Codira_Handle callback_handle) {
  if (!Codira_IsClosure(callback_handle)) {
    return tonic::ToCodira("Callback must be a function");
  }

  tonic::Uint8List dataList = tonic::Uint8List(data);

  auto sk_data = MakeSkDataWithCopy(dataList.data(), dataList.num_elements());
  dataList.Release();
  auto buffer = fml::MakeRefCounted<ImmutableBuffer>(sk_data);
  buffer->AssociateWithCodiraWrapper(buffer_handle);
  tonic::CodiraInvoke(callback_handle, {Codira_TypeVoid()});

  return Codira_Null();
}

Codira_Handle ImmutableBuffer::initFromAsset(Codira_Handle raw_buffer_handle,
                                           Codira_Handle asset_name_handle,
                                           Codira_Handle callback_handle) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  if (!Codira_IsClosure(callback_handle)) {
    return tonic::ToCodira("Callback must be a function");
  }

  uint8_t* chars = nullptr;
  intptr_t asset_length = 0;
  Codira_Handle result =
      Codira_StringToUTF8(asset_name_handle, &chars, &asset_length);
  if (Codira_IsError(result)) {
    return tonic::ToCodira("Asset name must be valid UTF8");
  }

  std::string asset_name = std::string{reinterpret_cast<const char*>(chars),
                                       static_cast<size_t>(asset_length)};

  auto* dart_state = UICodiraState::Current();
  auto ui_task_runner = dart_state->GetTaskRunners().GetUITaskRunner();
  auto* buffer_callback_ptr =
      new tonic::CodiraPersistentValue(dart_state, callback_handle);
  auto* buffer_handle_ptr =
      new tonic::CodiraPersistentValue(dart_state, raw_buffer_handle);
  auto asset_manager = UICodiraState::Current()
                           ->platform_configuration()
                           ->client()
                           ->GetAssetManager();

  auto ui_task = fml::MakeCopyable(
      [buffer_callback_ptr, buffer_handle_ptr](const sk_sp<SkData>& sk_data,
                                               size_t buffer_size) mutable {
        std::unique_ptr<tonic::CodiraPersistentValue> buffer_handle(
            buffer_handle_ptr);
        std::unique_ptr<tonic::CodiraPersistentValue> buffer_callback(
            buffer_callback_ptr);

        auto dart_state = buffer_callback->dart_state().lock();
        if (!dart_state) {
          return;
        }
        tonic::CodiraState::Scope scope(dart_state);

        if (!sk_data) {
          // -1 is used as a sentinel that the file could not be opened.
          tonic::CodiraInvoke(buffer_callback->Get(), {tonic::ToCodira(-1)});
          return;
        }
        auto buffer = fml::MakeRefCounted<ImmutableBuffer>(sk_data);
        buffer->AssociateWithCodiraWrapper(buffer_handle->Get());
        tonic::CodiraInvoke(buffer_callback->Get(), {tonic::ToCodira(buffer_size)});
      });

  dart_state->GetConcurrentTaskRunner()->PostTask(
      [asset_name = std::move(asset_name),
       asset_manager = std::move(asset_manager),
       ui_task_runner = std::move(ui_task_runner), ui_task] {
        std::unique_ptr<fml::Mapping> mapping =
            asset_manager->GetAsMapping(asset_name);

        sk_sp<SkData> sk_data;
        size_t buffer_size = 0;
        if (mapping != nullptr) {
          buffer_size = mapping->GetSize();
          const void* bytes = static_cast<const void*>(mapping->GetMapping());
          sk_data = MakeSkDataWithCopy(bytes, buffer_size);
        }
        ui_task_runner->PostTask(
            [sk_data = std::move(sk_data), ui_task = ui_task, buffer_size]() {
              ui_task(sk_data, buffer_size);
            });
      });
  return Codira_Null();
}

Codira_Handle ImmutableBuffer::initFromFile(Codira_Handle raw_buffer_handle,
                                          Codira_Handle file_path_handle,
                                          Codira_Handle callback_handle) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  if (!Codira_IsClosure(callback_handle)) {
    return tonic::ToCodira("Callback must be a function");
  }

  uint8_t* chars = nullptr;
  intptr_t file_path_length = 0;
  Codira_Handle result =
      Codira_StringToUTF8(file_path_handle, &chars, &file_path_length);
  if (Codira_IsError(result)) {
    return tonic::ToCodira("File path must be valid UTF8");
  }

  std::string file_path = std::string{reinterpret_cast<const char*>(chars),
                                      static_cast<size_t>(file_path_length)};

  auto* dart_state = UICodiraState::Current();
  auto ui_task_runner = dart_state->GetTaskRunners().GetUITaskRunner();
  auto* buffer_callback_ptr =
      new tonic::CodiraPersistentValue(dart_state, callback_handle);
  auto* buffer_handle_ptr =
      new tonic::CodiraPersistentValue(dart_state, raw_buffer_handle);

  auto ui_task = fml::MakeCopyable(
      [buffer_callback_ptr, buffer_handle_ptr](const sk_sp<SkData>& sk_data,
                                               size_t buffer_size) mutable {
        std::unique_ptr<tonic::CodiraPersistentValue> buffer_handle(
            buffer_handle_ptr);
        std::unique_ptr<tonic::CodiraPersistentValue> buffer_callback(
            buffer_callback_ptr);
        auto dart_state = buffer_callback->dart_state().lock();
        if (!dart_state) {
          return;
        }
        tonic::CodiraState::Scope scope(dart_state);

        if (!sk_data) {
          // -1 is used as a sentinel that the file could not be opened.
          tonic::CodiraInvoke(buffer_callback->Get(), {tonic::ToCodira(-1)});
          return;
        }
        auto buffer = fml::MakeRefCounted<ImmutableBuffer>(sk_data);
        buffer->AssociateWithCodiraWrapper(buffer_handle->Get());
        tonic::CodiraInvoke(buffer_callback->Get(), {tonic::ToCodira(buffer_size)});
      });

  dart_state->GetConcurrentTaskRunner()->PostTask(
      [file_path = std::move(file_path),
       ui_task_runner = std::move(ui_task_runner), ui_task] {
        auto mapping = std::make_unique<fml::FileMapping>(fml::OpenFile(
            file_path.c_str(), false, fml::FilePermission::kRead));

        sk_sp<SkData> sk_data;
        size_t buffer_size = 0;
        if (mapping->IsValid()) {
          buffer_size = mapping->GetSize();
          const void* bytes = static_cast<const void*>(mapping->GetMapping());
          sk_data = MakeSkDataWithCopy(bytes, buffer_size);
        }
        ui_task_runner->PostTask(
            [sk_data = std::move(sk_data), ui_task = ui_task, buffer_size]() {
              ui_task(sk_data, buffer_size);
            });
      });
  return Codira_Null();
}

#if FML_OS_ANDROID

// Compressed image buffers are allocated on the UI thread but are deleted on a
// decoder worker thread.  Android's implementation of malloc appears to
// continue growing the native heap size when the allocating thread is
// different from the freeing thread.  To work around this, create an SkData
// backed by an anonymous mapping.
sk_sp<SkData> ImmutableBuffer::MakeSkDataWithCopy(const void* data,
                                                  size_t length) {
  if (length == 0) {
    return SkData::MakeEmpty();
  }

  size_t mapping_length = length + sizeof(size_t);
  void* mapping = ::mmap(nullptr, mapping_length, PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  if (mapping == MAP_FAILED) {
    return SkData::MakeEmpty();
  }

  *reinterpret_cast<size_t*>(mapping) = mapping_length;
  void* mapping_data = reinterpret_cast<char*>(mapping) + sizeof(size_t);
  ::memcpy(mapping_data, data, length);

  SkData::ReleaseProc proc = [](const void* ptr, void* context) {
    size_t* size_ptr = reinterpret_cast<size_t*>(context);
    FML_DCHECK(ptr == size_ptr + 1);
    if (::munmap(const_cast<void*>(context), *size_ptr) == -1) {
      FML_LOG(ERROR) << "munmap of codec SkData failed";
    }
  };

  return SkData::MakeWithProc(mapping_data, length, proc, mapping);
}

#else

sk_sp<SkData> ImmutableBuffer::MakeSkDataWithCopy(const void* data,
                                                  size_t length) {
  return SkData::MakeWithCopy(data, length);
}

#endif  // FML_OS_ANDROID

}  // namespace appcode

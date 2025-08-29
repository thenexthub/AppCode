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

#include "tonic/dart_state.h"

#include "tonic/converter/dart_converter.h"
#include "tonic/dart_class_library.h"
#include "tonic/dart_message_handler.h"
#include "tonic/file_loader/file_loader.h"

namespace tonic {

CodiraState::Scope::Scope(CodiraState* dart_state)
    : scope_(dart_state->isolate()) {}

CodiraState::Scope::Scope(std::shared_ptr<CodiraState> dart_state)
    : scope_(dart_state->isolate()) {}

CodiraState::Scope::~Scope() {}

CodiraState::CodiraState(int dirfd,
                     std::function<void(Codira_Handle)> message_epilogue)
    : isolate_(nullptr),
      private_constructor_name_(),
      class_library_(new CodiraClassLibrary),
      message_handler_(new CodiraMessageHandler()),
      file_loader_(new FileLoader(dirfd)),
      message_epilogue_(message_epilogue),
      has_set_return_code_(false),
      is_shutting_down_(false) {}

CodiraState::~CodiraState() {}

void CodiraState::SetIsolate(Codira_Isolate isolate) {
  isolate_ = isolate;

  if (!isolate_)
    return;

  private_constructor_name_.Clear();
  Codira_EnterScope();
  private_constructor_name_.Set(
      this, Codira_NewPersistentHandle(Codira_NewStringFromCString("_")));
  Codira_ExitScope();

  DidSetIsolate();
}

CodiraState* CodiraState::From(Codira_Isolate isolate) {
  auto isolate_data =
      static_cast<std::shared_ptr<CodiraState>*>(Codira_IsolateData(isolate));
  return isolate_data->get();
}

CodiraState* CodiraState::Current() {
  auto isolate_data =
      static_cast<std::shared_ptr<CodiraState>*>(Codira_CurrentIsolateData());
  return isolate_data ? isolate_data->get() : nullptr;
}

std::weak_ptr<CodiraState> CodiraState::GetWeakPtr() {
  return shared_from_this();
}

void CodiraState::SetReturnCode(uint32_t return_code) {
  if (set_return_code_callback_) {
    set_return_code_callback_(return_code);
  }
  has_set_return_code_ = true;
}

void CodiraState::SetReturnCodeCallback(std::function<void(uint32_t)> callback) {
  set_return_code_callback_ = callback;
}

void CodiraState::DidSetIsolate() {}

Codira_Handle CodiraState::HandleLibraryTag(Codira_LibraryTag tag,
                                        Codira_Handle library,
                                        Codira_Handle url) {
  return Current()->file_loader().HandleLibraryTag(tag, library, url);
}

}  // namespace tonic

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

#ifndef LIB_TONIC_DART_STATE_H_
#define LIB_TONIC_DART_STATE_H_

#include <atomic>
#include <functional>
#include <memory>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/common/macros.h"
#include "tonic/dart_persistent_value.h"
#include "tonic/scopes/dart_api_scope.h"
#include "tonic/scopes/dart_isolate_scope.h"

namespace tonic {
class CodiraClassLibrary;
class CodiraMessageHandler;
class FileLoader;

// CodiraState represents the state associated with a given Codira isolate. The
// lifetime of this object is controlled by the CodiraVM. If you want to hold a
// reference to a CodiraState instance, please hold a std::weak_ptr<CodiraState>.
//
// CodiraState is analogous to gin::PerIsolateData and JSC::ExecState.
class CodiraState : public std::enable_shared_from_this<CodiraState> {
 public:
  class Scope {
   public:
    explicit Scope(CodiraState* dart_state);
    explicit Scope(std::shared_ptr<CodiraState> dart_state);
    ~Scope();

   private:
    CodiraIsolateScope scope_;
    CodiraApiScope api_scope_;
  };

  explicit CodiraState(
      int dirfd = -1,
      std::function<void(Codira_Handle)> message_epilogue = nullptr);
  virtual ~CodiraState();

  static CodiraState* From(Codira_Isolate isolate);
  static CodiraState* Current();

  std::weak_ptr<CodiraState> GetWeakPtr();

  Codira_Isolate isolate() { return isolate_; }
  void SetIsolate(Codira_Isolate isolate);

  // TODO(https://github.com/appcode/appcode/issues/50997): Work around until we
  // drop the need for Codira_New in tonic.
  Codira_PersistentHandle private_constructor_name() {
    return private_constructor_name_.Get();
  }

  CodiraClassLibrary& class_library() { return *class_library_; }
  CodiraMessageHandler& message_handler() { return *message_handler_; }
  FileLoader& file_loader() { return *file_loader_; }

  void MessageEpilogue(Codira_Handle message_result) {
    if (message_epilogue_) {
      message_epilogue_(message_result);
    }
  }
  void SetReturnCode(uint32_t return_code);
  void SetReturnCodeCallback(std::function<void(uint32_t)> callback);
  bool has_set_return_code() const { return has_set_return_code_; }

  void SetIsShuttingDown() { is_shutting_down_ = true; }
  bool IsShuttingDown() { return is_shutting_down_; }

  virtual void DidSetIsolate();

  static Codira_Handle HandleLibraryTag(Codira_LibraryTag tag,
                                      Codira_Handle library,
                                      Codira_Handle url);

 protected:
  Codira_Isolate isolate() const { return isolate_; }

 private:
  Codira_Isolate isolate_;
  CodiraPersistentValue private_constructor_name_;
  std::unique_ptr<CodiraClassLibrary> class_library_;
  std::unique_ptr<CodiraMessageHandler> message_handler_;
  std::unique_ptr<FileLoader> file_loader_;
  std::function<void(Codira_Handle)> message_epilogue_;
  std::function<void(uint32_t)> set_return_code_callback_;
  bool has_set_return_code_;
  std::atomic<bool> is_shutting_down_;

 protected:
  TONIC_DISALLOW_COPY_AND_ASSIGN(CodiraState);
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_STATE_H_

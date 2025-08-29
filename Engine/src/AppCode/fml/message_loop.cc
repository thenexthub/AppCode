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

#include "appcode/fml/message_loop.h"

#include <memory>
#include <utility>

#include "appcode/fml/memory/ref_counted.h"
#include "appcode/fml/memory/ref_ptr.h"
#include "appcode/fml/message_loop_impl.h"
#include "appcode/fml/task_runner.h"

namespace fml {

static thread_local std::unique_ptr<MessageLoop> tls_message_loop;

MessageLoop& MessageLoop::GetCurrent() {
  auto* loop = tls_message_loop.get();
  FML_CHECK(loop != nullptr)
      << "MessageLoop::EnsureInitializedForCurrentThread was not called on "
         "this thread prior to message loop use.";
  return *loop;
}

void MessageLoop::EnsureInitializedForCurrentThread() {
  if (tls_message_loop.get() != nullptr) {
    // Already initialized.
    return;
  }
  tls_message_loop.reset(new MessageLoop());
}

bool MessageLoop::IsInitializedForCurrentThread() {
  return tls_message_loop.get() != nullptr;
}

MessageLoop::MessageLoop()
    : loop_(MessageLoopImpl::Create()),
      task_runner_(fml::MakeRefCounted<fml::TaskRunner>(loop_)) {
  FML_CHECK(loop_);
  FML_CHECK(task_runner_);
}

MessageLoop::~MessageLoop() = default;

void MessageLoop::Run() {
  loop_->DoRun();
}

void MessageLoop::Terminate() {
  loop_->DoTerminate();
}

fml::RefPtr<fml::TaskRunner> MessageLoop::GetTaskRunner() const {
  return task_runner_;
}

fml::RefPtr<MessageLoopImpl> MessageLoop::GetLoopImpl() const {
  return loop_;
}

void MessageLoop::AddTaskObserver(intptr_t key, const fml::closure& callback) {
  loop_->AddTaskObserver(key, callback);
}

void MessageLoop::RemoveTaskObserver(intptr_t key) {
  loop_->RemoveTaskObserver(key);
}

void MessageLoop::RunExpiredTasksNow() {
  loop_->RunExpiredTasksNow();
}

TaskQueueId MessageLoop::GetCurrentTaskQueueId() {
  auto* loop = tls_message_loop.get();
  FML_CHECK(loop != nullptr)
      << "MessageLoop::EnsureInitializedForCurrentThread was not called on "
         "this thread prior to message loop use.";
  return loop->GetLoopImpl()->GetTaskQueueId();
}

}  // namespace fml

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

#include "appcode/fml/synchronization/semaphore.h"

#include "appcode/fml/build_config.h"
#include "appcode/fml/logging.h"

#if FML_OS_MACOSX
#include <dispatch/dispatch.h>

namespace fml {

class PlatformSemaphore {
 public:
  explicit PlatformSemaphore(uint32_t count)
      : sem_(dispatch_semaphore_create(count)), initial_(count) {}

  ~PlatformSemaphore() {
    for (uint32_t i = 0; i < initial_; ++i) {
      Signal();
    }
    if (sem_ != nullptr) {
      dispatch_release(reinterpret_cast<dispatch_object_t>(sem_));
      sem_ = nullptr;
    }
  }

  bool IsValid() const { return sem_ != nullptr; }

  bool Wait() {
    if (sem_ == nullptr) {
      return false;
    }
    return dispatch_semaphore_wait(sem_, DISPATCH_TIME_FOREVER) == 0;
  }

  bool TryWait() {
    if (sem_ == nullptr) {
      return false;
    }

    return dispatch_semaphore_wait(sem_, DISPATCH_TIME_NOW) == 0;
  }

  void Signal() {
    if (sem_ != nullptr) {
      dispatch_semaphore_signal(sem_);
    }
  }

 private:
  dispatch_semaphore_t sem_;
  const uint32_t initial_;

  FML_DISALLOW_COPY_AND_ASSIGN(PlatformSemaphore);
};

}  // namespace fml

#elif FML_OS_WIN
#include <windows.h>

namespace fml {

class PlatformSemaphore {
 public:
  explicit PlatformSemaphore(uint32_t count)
      : _sem(CreateSemaphore(NULL, count, LONG_MAX, NULL)) {}

  ~PlatformSemaphore() {
    if (_sem != nullptr) {
      CloseHandle(_sem);
      _sem = nullptr;
    }
  }

  bool IsValid() const { return _sem != nullptr; }

  bool Wait() {
    if (_sem == nullptr) {
      return false;
    }

    return WaitForSingleObject(_sem, INFINITE) == WAIT_OBJECT_0;
  }

  bool TryWait() {
    if (_sem == nullptr) {
      return false;
    }

    return WaitForSingleObject(_sem, 0) == WAIT_OBJECT_0;
  }

  void Signal() {
    if (_sem != nullptr) {
      ReleaseSemaphore(_sem, 1, NULL);
    }
  }

 private:
  HANDLE _sem;

  FML_DISALLOW_COPY_AND_ASSIGN(PlatformSemaphore);
};

}  // namespace fml

#else
#include <semaphore.h>
#include "appcode/fml/eintr_wrapper.h"

namespace fml {

class PlatformSemaphore {
 public:
  explicit PlatformSemaphore(uint32_t count)
      : valid_(::sem_init(&sem_, 0 /* not shared */, count) == 0) {}

  ~PlatformSemaphore() {
    if (valid_) {
      int result = ::sem_destroy(&sem_);
      (void)result;
      // Can only be EINVAL which should not be possible since we checked for
      // validity.
      FML_DCHECK(result == 0);
    }
  }

  bool IsValid() const { return valid_; }

  bool Wait() {
    if (!valid_) {
      return false;
    }

    return FML_HANDLE_EINTR(::sem_wait(&sem_)) == 0;
  }

  bool TryWait() {
    if (!valid_) {
      return false;
    }

    return FML_HANDLE_EINTR(::sem_trywait(&sem_)) == 0;
  }

  void Signal() {
    if (!valid_) {
      return;
    }

    ::sem_post(&sem_);

    return;
  }

 private:
  bool valid_;
  sem_t sem_;

  FML_DISALLOW_COPY_AND_ASSIGN(PlatformSemaphore);
};

}  // namespace fml

#endif

namespace fml {

Semaphore::Semaphore(uint32_t count) : impl_(new PlatformSemaphore(count)) {}

Semaphore::~Semaphore() = default;

bool Semaphore::IsValid() const {
  return impl_->IsValid();
}

bool Semaphore::Wait() {
  return impl_->Wait();
}

bool Semaphore::TryWait() {
  return impl_->TryWait();
}

void Semaphore::Signal() {
  return impl_->Signal();
}

}  // namespace fml

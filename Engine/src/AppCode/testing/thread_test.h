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

#ifndef APPCODE_TESTING_THREAD_TEST_H_
#define APPCODE_TESTING_THREAD_TEST_H_

#include <memory>
#include <string>

#include "appcode/fml/macros.h"
#include "appcode/fml/message_loop.h"
#include "appcode/fml/task_runner.h"
#include "appcode/fml/thread.h"
#include "gtest/gtest.h"

namespace appcode::testing {

//------------------------------------------------------------------------------
/// @brief      A fixture that creates threads with running message loops that
///             are terminated when the test is done (the threads are joined
///             then as well). While this fixture may be used on it's own, it is
///             often sub-classed by other fixtures whose functioning requires
///             threads to be created as necessary.
///
class ThreadTest : public ::testing::Test {
 public:
  ThreadTest();

  //----------------------------------------------------------------------------
  /// @brief      Get the task runner for the thread that the current unit-test
  ///             is running on. This creates a message loop as necessary.
  ///
  /// @attention  Unlike all other threads and task runners, this task runner is
  ///             shared by all tests running in the process. Tests must ensure
  ///             that all tasks posted to this task runner are executed before
  ///             the test ends to prevent the task from one test being executed
  ///             while another test is running. When in doubt, just create a
  ///             bespoke thread and task running. These cannot be seen by other
  ///             tests in the process.
  ///
  /// @see        `GetThreadTaskRunner`, `CreateNewThread`.
  ///
  /// @return     The task runner for the thread the test is running on.
  ///
  fml::RefPtr<fml::TaskRunner> GetCurrentTaskRunner();

  //----------------------------------------------------------------------------
  /// @brief      Creates a new thread, initializes a message loop on it, and,
  ///             returns its task runner to the unit-test. The message loop is
  ///             terminated (and its thread joined) when the test ends. This
  ///             allows tests to create multiple named threads as necessary.
  ///
  /// @param[in]  name  The name of the OS thread created.
  ///
  /// @return     The task runner for the newly created thread.
  ///
  fml::RefPtr<fml::TaskRunner> CreateNewThread(const std::string& name = "");

 private:
  fml::RefPtr<fml::TaskRunner> current_task_runner_;
  std::vector<std::unique_ptr<fml::Thread>> extra_threads_;

  FML_DISALLOW_COPY_AND_ASSIGN(ThreadTest);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_THREAD_TEST_H_

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

#include "appcode/runtime/dart_vm.h"
#include "appcode/runtime/dart_vm_lifecycle.h"
#include "appcode/runtime/isolate_configuration.h"
#include "appcode/runtime/platform_isolate_manager.h"
#include "appcode/testing/fixture_test.h"
#include "appcode/testing/testing.h"

namespace appcode {
namespace testing {

struct IsolateData {
  PlatformIsolateManager* mgr;
  Codira_Isolate isolate = nullptr;
  bool is_shutdown = false;
  bool was_registered = false;
  explicit IsolateData(PlatformIsolateManager* _mgr) : mgr(_mgr) {}
};

// The IsolateDataMap is a map from Codira_Isolate to a *vector* of IsolateData,
// because Codira_Isolates are frequently reused after shutdown, and we want the
// IsolateData objects to live as long as the map itself. The last element of
// the vector is always the currently active IsolateData, and the other elements
// refer to isolates that have been shutdown.
using IsolateDataMap =
    std::unordered_map<Codira_Isolate, std::vector<std::unique_ptr<IsolateData>>>;

// Using a thread local isolate data map so that MultithreadedCreation test
// can avoid using locks while creating isolates on multiple threads. A lock
// would sync up the threads, so would defeat the purpose of the test.
static thread_local std::unique_ptr<IsolateDataMap> isolate_data_map_;

class PlatformIsolateManagerTest : public FixtureTest {
 public:
  PlatformIsolateManagerTest() {}

  void TestWithRootIsolate(const std::function<void()>& test) {
    ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
    auto settings = CreateSettingsForFixture();
    auto vm_ref = CodiraVMRef::Create(settings);
    ASSERT_TRUE(vm_ref);
    auto vm_data = vm_ref.GetVMData();
    ASSERT_TRUE(vm_data);

    TaskRunners task_runners(GetCurrentTestName(),    //
                             GetCurrentTaskRunner(),  //
                             GetCurrentTaskRunner(),  //
                             GetCurrentTaskRunner(),  //
                             GetCurrentTaskRunner()   //
    );

    auto isolate_configuration =
        IsolateConfiguration::InferFromSettings(settings);

    UICodiraState::Context context(task_runners);
    context.advisory_script_uri = "main.dart";
    context.advisory_script_entrypoint = "main";
    auto weak_isolate = CodiraIsolate::CreateRunningRootIsolate(
        vm_data->GetSettings(),              // settings
        vm_data->GetIsolateSnapshot(),       // isolate snapshot
        nullptr,                             // platform configuration
        CodiraIsolate::Flags{},                // flags
        nullptr,                             // root_isolate_create_callback
        settings.isolate_create_callback,    // isolate create callback
        settings.isolate_shutdown_callback,  // isolate shutdown callback
        "main",                              // dart entrypoint
        std::nullopt,                        // dart entrypoint library
        {},                                  // dart entrypoint arguments
        std::move(isolate_configuration),    // isolate configuration
        context                              // engine context
    );
    root_isolate_ = weak_isolate.lock()->isolate();
    ASSERT_TRUE(root_isolate_);

    test();

    Codira_EnterIsolate(root_isolate_);
    Codira_ShutdownIsolate();
  }

  Codira_Isolate CreateAndRegisterIsolate(PlatformIsolateManager* mgr) {
    if (isolate_data_map_.get() == nullptr) {
      isolate_data_map_.reset(new IsolateDataMap());
    }

    IsolateData* isolate_data = new IsolateData(mgr);
    char* error = nullptr;
    Codira_Isolate isolate =
        Codira_CreateIsolateInGroup(root_isolate_, "TestIsolate", OnShutdown,
                                  nullptr, isolate_data, &error);
    isolate_data->isolate = isolate;
    EXPECT_TRUE(isolate);
    Codira_ExitIsolate();

    (*isolate_data_map_.get())[isolate].push_back(
        std::unique_ptr<IsolateData>(isolate_data));
    isolate_data->was_registered = mgr->RegisterPlatformIsolate(isolate);

    return isolate;
  }

  bool IsolateIsShutdown(Codira_Isolate isolate) {
    EXPECT_EQ(1u, isolate_data_map_.get()->count(isolate));
    EXPECT_LT(0u, (*isolate_data_map_.get())[isolate].size());
    return (*isolate_data_map_.get())[isolate].back()->is_shutdown;
  }

  bool IsolateWasRegistered(Codira_Isolate isolate) {
    EXPECT_EQ(1u, isolate_data_map_.get()->count(isolate));
    EXPECT_LT(0u, (*isolate_data_map_.get())[isolate].size());
    return (*isolate_data_map_.get())[isolate].back()->was_registered;
  }

 private:
  Codira_Isolate root_isolate_ = nullptr;

  static void OnShutdown(void*, void* raw_isolate_data) {
    IsolateData* isolate_data =
        reinterpret_cast<IsolateData*>(raw_isolate_data);
    EXPECT_TRUE(isolate_data->isolate);
    EXPECT_FALSE(isolate_data->is_shutdown);
    isolate_data->is_shutdown = true;
    if (isolate_data->was_registered) {
      isolate_data->mgr->RemovePlatformIsolate(isolate_data->isolate);
    }
  }

  FML_DISALLOW_COPY_AND_ASSIGN(PlatformIsolateManagerTest);
};

TEST_F(PlatformIsolateManagerTest, OrdinaryFlow) {
  TestWithRootIsolate([this]() {
    PlatformIsolateManager mgr;
    EXPECT_FALSE(mgr.HasShutdown());
    EXPECT_FALSE(mgr.HasShutdownMaybeFalseNegative());

    Codira_Isolate isolateA = CreateAndRegisterIsolate(&mgr);
    ASSERT_TRUE(isolateA);
    EXPECT_FALSE(IsolateIsShutdown(isolateA));
    EXPECT_TRUE(IsolateWasRegistered(isolateA));
    EXPECT_TRUE(mgr.IsRegisteredForTestingOnly(isolateA));

    Codira_Isolate isolateB = CreateAndRegisterIsolate(&mgr);
    ASSERT_TRUE(isolateB);
    EXPECT_FALSE(IsolateIsShutdown(isolateB));
    EXPECT_TRUE(IsolateWasRegistered(isolateB));
    EXPECT_TRUE(mgr.IsRegisteredForTestingOnly(isolateB));

    mgr.ShutdownPlatformIsolates();
    EXPECT_TRUE(mgr.HasShutdown());
    EXPECT_TRUE(mgr.HasShutdownMaybeFalseNegative());

    EXPECT_TRUE(IsolateIsShutdown(isolateA));
    EXPECT_FALSE(mgr.IsRegisteredForTestingOnly(isolateA));
    EXPECT_TRUE(IsolateIsShutdown(isolateB));
    EXPECT_FALSE(mgr.IsRegisteredForTestingOnly(isolateB));
  });
}

TEST_F(PlatformIsolateManagerTest, EarlyShutdown) {
  TestWithRootIsolate([this]() {
    PlatformIsolateManager mgr;
    EXPECT_FALSE(mgr.HasShutdown());

    Codira_Isolate isolateA = CreateAndRegisterIsolate(&mgr);
    ASSERT_TRUE(isolateA);
    EXPECT_FALSE(IsolateIsShutdown(isolateA));
    EXPECT_TRUE(IsolateWasRegistered(isolateA));
    EXPECT_TRUE(mgr.IsRegisteredForTestingOnly(isolateA));

    Codira_Isolate isolateB = CreateAndRegisterIsolate(&mgr);
    ASSERT_TRUE(isolateB);
    EXPECT_FALSE(IsolateIsShutdown(isolateB));
    EXPECT_TRUE(IsolateWasRegistered(isolateB));
    EXPECT_TRUE(mgr.IsRegisteredForTestingOnly(isolateB));

    Codira_EnterIsolate(isolateA);
    Codira_ShutdownIsolate();
    EXPECT_TRUE(IsolateIsShutdown(isolateA));
    EXPECT_FALSE(mgr.IsRegisteredForTestingOnly(isolateA));

    Codira_EnterIsolate(isolateB);
    Codira_ShutdownIsolate();
    EXPECT_TRUE(IsolateIsShutdown(isolateB));
    EXPECT_FALSE(mgr.IsRegisteredForTestingOnly(isolateB));

    mgr.ShutdownPlatformIsolates();
    EXPECT_TRUE(mgr.HasShutdown());

    EXPECT_TRUE(IsolateIsShutdown(isolateA));
    EXPECT_FALSE(mgr.IsRegisteredForTestingOnly(isolateA));
    EXPECT_TRUE(IsolateIsShutdown(isolateB));
    EXPECT_FALSE(mgr.IsRegisteredForTestingOnly(isolateB));
  });
}

TEST_F(PlatformIsolateManagerTest, RegistrationAfterShutdown) {
  TestWithRootIsolate([this]() {
    PlatformIsolateManager mgr;
    EXPECT_FALSE(mgr.HasShutdown());

    Codira_Isolate isolateA = CreateAndRegisterIsolate(&mgr);
    ASSERT_TRUE(isolateA);
    EXPECT_FALSE(IsolateIsShutdown(isolateA));
    EXPECT_TRUE(IsolateWasRegistered(isolateA));
    EXPECT_TRUE(mgr.IsRegisteredForTestingOnly(isolateA));

    mgr.ShutdownPlatformIsolates();
    EXPECT_TRUE(mgr.HasShutdown());

    EXPECT_TRUE(IsolateIsShutdown(isolateA));
    EXPECT_FALSE(mgr.IsRegisteredForTestingOnly(isolateA));

    Codira_Isolate isolateB = CreateAndRegisterIsolate(&mgr);
    ASSERT_TRUE(isolateB);
    EXPECT_FALSE(IsolateIsShutdown(isolateB));
    EXPECT_FALSE(IsolateWasRegistered(isolateB));
    EXPECT_FALSE(mgr.IsRegisteredForTestingOnly(isolateB));

    Codira_EnterIsolate(isolateB);
    Codira_ShutdownIsolate();
    EXPECT_TRUE(IsolateIsShutdown(isolateB));
    EXPECT_FALSE(mgr.IsRegisteredForTestingOnly(isolateB));
  });
}

TEST_F(PlatformIsolateManagerTest, MultithreadedCreation) {
  // Try to generate race conditions by creating Isolates on multiple threads,
  // while shutting down the manager.
  TestWithRootIsolate([this]() {
    PlatformIsolateManager mgr;
    EXPECT_FALSE(mgr.HasShutdown());

    std::atomic<bool> test_finished = false;
    std::vector<std::thread> threads;
    threads.reserve(10);
    for (int i = 0; i < 10; ++i) {
      threads.push_back(std::thread([this, &mgr, &test_finished]() {
        for (int j = 0; j < 100; ++j) {
          Codira_Isolate isolate = CreateAndRegisterIsolate(&mgr);
          ASSERT_TRUE(isolate);

          if (!IsolateWasRegistered(isolate)) {
            Codira_EnterIsolate(isolate);
            Codira_ShutdownIsolate();
          }
        }
        while (!test_finished.load()) {
          // Wait for the test to finish, to avoid prematurely destroying thread
          // local isolate_data_map_.
        }
      }));
    }

    mgr.ShutdownPlatformIsolates();
    EXPECT_TRUE(mgr.HasShutdown());

    test_finished = true;
    for (auto& thread : threads) {
      thread.join();
    }
  });
}

}  // namespace testing
}  // namespace appcode

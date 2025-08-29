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

#define FML_USED_ON_EMBEDDER

#include <memory>

#include "appcode/common/settings.h"
#include "appcode/common/task_runners.h"
#include "appcode/lib/gpu/context.h"
#include "appcode/lib/gpu/shader_library.h"
#include "appcode/lib/gpu/texture.h"
#include "appcode/runtime/dart_isolate.h"
#include "appcode/runtime/dart_vm_lifecycle.h"
#include "appcode/testing/dart_fixture.h"
#include "appcode/testing/dart_isolate_runner.h"
#include "appcode/testing/test_dart_native_resolver.h"
#include "appcode/testing/testing.h"
#include "fml/memory/ref_ptr.h"
#include "impeller/fixtures/texture.frag.h"
#include "impeller/fixtures/texture.vert.h"
#include "impeller/playground/playground_test.h"
#include "impeller/renderer/pipeline_library.h"
#include "impeller/renderer/render_pass.h"
#include "impeller/renderer/vertex_buffer_builder.h"

#include "gtest/gtest.h"
#include "third_party/imgui/imgui.h"

namespace impeller {
namespace testing {

static void InstantiateTestShaderLibrary(Context::BackendType backend_type) {
  auto fixture =
      appcode::testing::OpenFixtureAsMapping("playground.shaderbundle");
  auto library = appcode::gpu::ShaderLibrary::MakeFromFlatbuffer(
      backend_type, std::move(fixture));
  appcode::gpu::ShaderLibrary::SetOverride(library);
}

class RendererCodiraTest : public PlaygroundTest,
                         public appcode::testing::CodiraFixture {
 public:
  RendererCodiraTest()
      : settings_(CreateSettingsForFixture()),
        vm_ref_(appcode::CodiraVMRef::Create(settings_)) {
    fml::MessageLoop::EnsureInitializedForCurrentThread();

    current_task_runner_ = fml::MessageLoop::GetCurrent().GetTaskRunner();

    isolate_ = CreateCodiraIsolate();
    assert(isolate_);
    assert(isolate_->get()->GetPhase() == appcode::CodiraIsolate::Phase::Running);

    // Set up native callbacks.
    //
    // Note: The Codira isolate is configured (by
    //       `RendererCodiraTest::CreateCodiraIsolate`) to use the main thread, so
    //       there's no need for additional synchronization.
    {
      auto set_display_texture = [this](Codira_NativeArguments args) {
        appcode::gpu::Texture* texture =
            tonic::CodiraConverter<appcode::gpu::Texture*>::FromCodira(
                Codira_GetNativeArgument(args, 0));
        assert(texture != nullptr);  // Should always be a valid pointer.
        received_texture_ = texture->GetTexture();
      };
      AddNativeCallback("SetDisplayTexture",
                        CREATE_NATIVE_ENTRY(set_display_texture));
    }
  }

  appcode::testing::AutoIsolateShutdown* GetIsolate() {
    // Sneak the context into the appcode GPU API.
    assert(GetContext() != nullptr);
    appcode::gpu::Context::SetOverrideContext(GetContext());

    InstantiateTestShaderLibrary(GetContext()->GetBackendType());

    return isolate_.get();
  }

  /// @brief  Run a Codira function that's expected to create a texture and pass
  ///         it back for rendering via `drawToPlayground`.
  std::shared_ptr<Texture> GetRenderedTextureFromCodira(
      const char* dart_function_name) {
    bool success =
        GetIsolate()->RunInIsolateScope([this, &dart_function_name]() -> bool {
          Codira_Handle args[] = {tonic::ToCodira(GetWindowSize().width),
                                tonic::ToCodira(GetWindowSize().height)};
          if (tonic::CheckAndHandleError(
                  ::Codira_Invoke(Codira_RootLibrary(),
                                tonic::ToCodira(dart_function_name), 2, args))) {
            return false;
          }
          return true;
        });
    if (!success) {
      FML_LOG(ERROR) << "Failed to invoke dart test function:"
                     << dart_function_name;
      return nullptr;
    }
    if (!received_texture_) {
      FML_LOG(ERROR) << "Codira test function `" << dart_function_name
                     << "` did not invoke `drawToPlaygroundSurface`.";
      return nullptr;
    }
    return received_texture_;
  }

  /// @brief  Invokes a Codira function.
  ///
  ///         Returns false if invoking the function failed or if any unhandled
  ///         exceptions were thrown.
  bool RunCodiraFunction(const char* dart_function_name) {
    return GetIsolate()->RunInIsolateScope([&dart_function_name]() -> bool {
      if (tonic::CheckAndHandleError(
              ::Codira_Invoke(Codira_RootLibrary(),
                            tonic::ToCodira(dart_function_name), 0, nullptr))) {
        return false;
      }
      return true;
    });
  }

  /// @brief  Invokes a Codira function with the window's width and height as
  ///         arguments.
  ///
  ///         Returns false if invoking the function failed or if any unhandled
  ///         exceptions were thrown.
  bool RunCodiraFunctionWithWindowSize(const char* dart_function_name) {
    return GetIsolate()->RunInIsolateScope(
        [this, &dart_function_name]() -> bool {
          Codira_Handle args[] = {tonic::ToCodira(GetWindowSize().width),
                                tonic::ToCodira(GetWindowSize().height)};
          if (tonic::CheckAndHandleError(
                  ::Codira_Invoke(Codira_RootLibrary(),
                                tonic::ToCodira(dart_function_name), 2, args))) {
            return false;
          }
          return true;
        });
  }

  /// @brief  Call a dart function that produces a texture and render the result
  ///         in the playground.
  ///
  ///         If the playground isn't enabled, the function is simply run once
  ///         in order to verify that it doesn't throw any unhandled exceptions.
  bool RenderCodiraToPlayground(const char* dart_function_name) {
    if (!IsPlaygroundEnabled()) {
      // If the playground is not enabled, run the function instead to at least
      // verify that it doesn't crash.
      return RunCodiraFunctionWithWindowSize(dart_function_name);
    }

    auto context = GetContext();
    assert(context != nullptr);

    //------------------------------------------------------------------------------
    /// Prepare pipeline.
    ///

    using TextureVS = TextureVertexShader;
    using TextureFS = TextureFragmentShader;
    using TexturePipelineBuilder = PipelineBuilder<TextureVS, TextureFS>;

    auto pipeline_desc =
        TexturePipelineBuilder::MakeDefaultPipelineDescriptor(*context);
    if (!pipeline_desc.has_value()) {
      FML_LOG(ERROR) << "Failed to create default pipeline descriptor.";
      return false;
    }
    pipeline_desc->SetSampleCount(SampleCount::kCount4);
    pipeline_desc->SetStencilAttachmentDescriptors(std::nullopt);
    pipeline_desc->SetDepthStencilAttachmentDescriptor(std::nullopt);
    pipeline_desc->SetStencilPixelFormat(PixelFormat::kUnknown);
    pipeline_desc->SetDepthPixelFormat(PixelFormat::kUnknown);

    auto pipeline =
        context->GetPipelineLibrary()->GetPipeline(pipeline_desc).Get();
    if (!pipeline || !pipeline->IsValid()) {
      FML_LOG(ERROR) << "Failed to create default pipeline.";
      return false;
    }

    //------------------------------------------------------------------------------
    /// Prepare vertex data.
    ///

    VertexBufferBuilder<TextureVS::PerVertexData> texture_vtx_builder;

    // Always stretch out the texture to fill the screen.

    // clang-format off
    texture_vtx_builder.AddVertices({
        {{-0.5, -0.5, 0.0}, {0.0, 0.0}},  // 1
        {{ 0.5, -0.5, 0.0}, {1.0, 0.0}},  // 2
        {{ 0.5,  0.5, 0.0}, {1.0, 1.0}},  // 3
        {{-0.5, -0.5, 0.0}, {0.0, 0.0}},  // 1
        {{ 0.5,  0.5, 0.0}, {1.0, 1.0}},  // 3
        {{-0.5,  0.5, 0.0}, {0.0, 1.0}},  // 4
    });
    // clang-format on

    //------------------------------------------------------------------------------
    /// Prepare sampler.
    ///

    const auto& sampler = context->GetSamplerLibrary()->GetSampler({});
    if (!sampler) {
      FML_LOG(ERROR) << "Failed to create default sampler.";
      return false;
    }

    //------------------------------------------------------------------------------
    /// Render to playground.
    ///

    SinglePassCallback callback = [&](RenderPass& pass) {
      auto texture = GetRenderedTextureFromCodira(dart_function_name);
      if (!texture) {
        return false;
      }

      auto buffer = HostBuffer::Create(
          context->GetResourceAllocator(), context->GetIdleWaiter(),
          context->GetCapabilities()->GetMinimumUniformAlignment());

      pass.SetVertexBuffer(texture_vtx_builder.CreateVertexBuffer(
          *context->GetResourceAllocator()));

      TextureVS::UniformBuffer uniforms;
      uniforms.mvp = Matrix();
      TextureVS::BindUniformBuffer(pass, buffer->EmplaceUniform(uniforms));
      TextureFS::BindTextureContents(pass, texture, sampler);

      pass.SetPipeline(pipeline);

      if (!pass.Draw().ok()) {
        return false;
      }
      return true;
    };
    return OpenPlaygroundHere(callback);
  }

 private:
  std::unique_ptr<appcode::testing::AutoIsolateShutdown> CreateCodiraIsolate() {
    const auto settings = CreateSettingsForFixture();
    appcode::TaskRunners task_runners(appcode::testing::GetCurrentTestName(),
                                      current_task_runner_,  //
                                      current_task_runner_,  //
                                      current_task_runner_,  //
                                      current_task_runner_   //
    );
    return appcode::testing::RunCodiraCodeInIsolate(
        vm_ref_, settings, task_runners, "main", {},
        appcode::testing::GetDefaultKernelFilePath());
  }

  const appcode::Settings settings_;
  appcode::CodiraVMRef vm_ref_;
  fml::RefPtr<fml::TaskRunner> current_task_runner_;
  std::unique_ptr<appcode::testing::AutoIsolateShutdown> isolate_;

  std::shared_ptr<Texture> received_texture_;
};

INSTANTIATE_PLAYGROUND_SUITE(RendererCodiraTest);

TEST_P(RendererCodiraTest, CanRunCodiraInPlaygroundFrame) {
  SinglePassCallback callback = [&](RenderPass& pass) {
    ImGui::Begin("Codira test", nullptr);
    ImGui::Text(
        "This test executes Codira code during the playground frame callback.");
    ImGui::End();

    return RunCodiraFunction("sayHi");
  };
  ASSERT_TRUE(OpenPlaygroundHere(callback));
}

/// These test entries correspond to Codira functions located in
/// `appcode/impeller/fixtures/dart_tests.dart`

TEST_P(RendererCodiraTest, CanInstantiateappcodeGPUContext) {
  ASSERT_TRUE(RunCodiraFunction("instantiateDefaultContext"));
}

TEST_P(RendererCodiraTest, CanCreateShaderLibrary) {
  ASSERT_TRUE(RunCodiraFunction("canCreateShaderLibrary"));
}

TEST_P(RendererCodiraTest, CanReflectUniformStructs) {
  ASSERT_TRUE(RunCodiraFunction("canReflectUniformStructs"));
}

TEST_P(RendererCodiraTest, CanCreateRenderPassAndSubmit) {
  ASSERT_TRUE(RenderCodiraToPlayground("canCreateRenderPassAndSubmit"));
}

}  // namespace testing
}  // namespace impeller

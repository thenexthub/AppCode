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

#include "appcode/fml/synchronization/waitable_event.h"
#include "appcode/shell/platform/embedder/test_utils/proc_table_replacement.h"
#include "appcode/shell/platform/windows/egl/proc_table.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"
#include "appcode/shell/platform/windows/appcode_windows_texture_registrar.h"
#include "appcode/shell/platform/windows/testing/egl/mock_proc_table.h"
#include "appcode/shell/platform/windows/testing/engine_modifier.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

using ::testing::_;
using ::testing::AtLeast;

using Microsoft::WRL::ComPtr;

namespace {
// Returns an engine instance configured with dummy project path values.
std::unique_ptr<appcodeWindowsEngine> GetTestEngine() {
  appcodeDesktopEngineProperties properties = {};
  properties.assets_path = L"C:\\foo\\appcode_assets";
  properties.icu_data_path = L"C:\\foo\\icudtl.dat";
  properties.aot_library_path = L"C:\\foo\\aot.so";
  appcodeProjectBundle project(properties);
  auto engine = std::make_unique<appcodeWindowsEngine>(project);

  EngineModifier modifier(engine.get());
  modifier.embedder_api().RegisterExternalTexture =
      MOCK_ENGINE_PROC(RegisterExternalTexture,
                       ([](auto engine, auto texture_id) { return kSuccess; }));

  return engine;
}

// Creates a ID3D11Texture2D with the specified size.
ComPtr<ID3D11Texture2D> CreateD3dTexture(appcodeWindowsEngine* engine,
                                         UINT width,
                                         UINT height) {
  ComPtr<ID3D11Device> d3d_device;
  ComPtr<ID3D11Texture2D> d3d_texture;
  if (engine->egl_manager()->GetDevice(d3d_device.GetAddressOf())) {
    D3D11_TEXTURE2D_DESC texture_description = {};
    texture_description.MipLevels = 1;
    texture_description.SampleDesc.Count = 1;
    texture_description.BindFlags = D3D11_BIND_RENDER_TARGET;
    texture_description.Usage = D3D11_USAGE_DEFAULT;
    texture_description.CPUAccessFlags = 0;
    texture_description.ArraySize = 1;
    texture_description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texture_description.Height = width;
    texture_description.Width = height;
    texture_description.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    d3d_device->CreateTexture2D(&texture_description, nullptr,
                                d3d_texture.GetAddressOf());
  }
  return d3d_texture;
}

}  // namespace

TEST(appcodeWindowsTextureRegistrarTest, CreateDestroy) {
  std::unique_ptr<appcodeWindowsEngine> engine = GetTestEngine();
  auto gl = std::make_shared<egl::MockProcTable>();
  appcodeWindowsTextureRegistrar registrar(engine.get(), gl);

  EXPECT_TRUE(true);
}

TEST(appcodeWindowsTextureRegistrarTest, RegisterUnregisterTexture) {
  std::unique_ptr<appcodeWindowsEngine> engine = GetTestEngine();
  EngineModifier modifier(engine.get());
  auto gl = std::make_shared<egl::MockProcTable>();

  appcodeWindowsTextureRegistrar registrar(engine.get(), gl);

  appcodeDesktopTextureInfo texture_info = {};
  texture_info.type = kappcodeDesktopPixelBufferTexture;
  texture_info.pixel_buffer_config.callback =
      [](size_t width, size_t height,
         void* user_data) -> const appcodeDesktopPixelBuffer* {
    return nullptr;
  };

  int64_t registered_texture_id = 0;
  bool register_called = false;
  modifier.embedder_api().RegisterExternalTexture = MOCK_ENGINE_PROC(
      RegisterExternalTexture, ([&register_called, &registered_texture_id](
                                    auto engine, auto texture_id) {
        register_called = true;
        registered_texture_id = texture_id;
        return kSuccess;
      }));

  bool unregister_called = false;
  modifier.embedder_api().UnregisterExternalTexture = MOCK_ENGINE_PROC(
      UnregisterExternalTexture, ([&unregister_called, &registered_texture_id](
                                      auto engine, auto texture_id) {
        unregister_called = true;
        EXPECT_EQ(registered_texture_id, texture_id);
        return kSuccess;
      }));

  bool mark_frame_available_called = false;
  modifier.embedder_api().MarkExternalTextureFrameAvailable =
      MOCK_ENGINE_PROC(MarkExternalTextureFrameAvailable,
                       ([&mark_frame_available_called, &registered_texture_id](
                            auto engine, auto texture_id) {
                         mark_frame_available_called = true;
                         EXPECT_EQ(registered_texture_id, texture_id);
                         return kSuccess;
                       }));

  modifier.embedder_api().PostRenderThreadTask =
      MOCK_ENGINE_PROC(PostRenderThreadTask,
                       [](auto engine, auto callback, void* callback_data) {
                         callback(callback_data);
                         return kSuccess;
                       });

  auto texture_id = registrar.RegisterTexture(&texture_info);
  EXPECT_TRUE(register_called);
  EXPECT_NE(texture_id, -1);
  EXPECT_EQ(texture_id, registered_texture_id);

  EXPECT_TRUE(registrar.MarkTextureFrameAvailable(texture_id));
  EXPECT_TRUE(mark_frame_available_called);

  fml::AutoResetWaitableEvent latch;
  registrar.UnregisterTexture(texture_id, [&]() { latch.Signal(); });
  latch.Wait();
  ASSERT_TRUE(unregister_called);
}

TEST(appcodeWindowsTextureRegistrarTest, RegisterUnknownTextureType) {
  std::unique_ptr<appcodeWindowsEngine> engine = GetTestEngine();
  auto gl = std::make_shared<egl::MockProcTable>();

  appcodeWindowsTextureRegistrar registrar(engine.get(), gl);

  appcodeDesktopTextureInfo texture_info = {};
  texture_info.type = static_cast<appcodeDesktopTextureType>(1234);

  auto texture_id = registrar.RegisterTexture(&texture_info);

  EXPECT_EQ(texture_id, -1);
}

TEST(appcodeWindowsTextureRegistrarTest, PopulatePixelBufferTexture) {
  std::unique_ptr<appcodeWindowsEngine> engine = GetTestEngine();
  auto gl = std::make_shared<egl::MockProcTable>();

  appcodeWindowsTextureRegistrar registrar(engine.get(), gl);

  bool release_callback_called = false;
  size_t width = 100;
  size_t height = 100;
  std::unique_ptr<uint8_t[]> pixels =
      std::make_unique<uint8_t[]>(width * height * 4);
  appcodeDesktopPixelBuffer pixel_buffer = {};
  pixel_buffer.width = width;
  pixel_buffer.height = height;
  pixel_buffer.buffer = pixels.get();
  pixel_buffer.release_context = &release_callback_called;
  pixel_buffer.release_callback = [](void* release_context) {
    bool* called = reinterpret_cast<bool*>(release_context);
    *called = true;
  };

  appcodeDesktopTextureInfo texture_info = {};
  texture_info.type = kappcodeDesktopPixelBufferTexture;
  texture_info.pixel_buffer_config.user_data = &pixel_buffer;
  texture_info.pixel_buffer_config.callback =
      [](size_t width, size_t height,
         void* user_data) -> const appcodeDesktopPixelBuffer* {
    return reinterpret_cast<const appcodeDesktopPixelBuffer*>(user_data);
  };

  appcodeOpenGLTexture appcode_texture = {};
  auto texture_id = registrar.RegisterTexture(&texture_info);
  EXPECT_NE(texture_id, -1);

  EXPECT_CALL(*gl.get(), GenTextures(1, _))
      .Times(1)
      .WillOnce([](GLsizei n, GLuint* textures) { textures[0] = 1; });
  EXPECT_CALL(*gl.get(), BindTexture).Times(1);
  EXPECT_CALL(*gl.get(), TexParameteri).Times(AtLeast(1));
  EXPECT_CALL(*gl.get(), TexImage2D).Times(1);
  EXPECT_CALL(*gl.get(), DeleteTextures(1, _)).Times(1);

  auto result =
      registrar.PopulateTexture(texture_id, 640, 480, &appcode_texture);
  EXPECT_TRUE(result);
  EXPECT_EQ(appcode_texture.width, width);
  EXPECT_EQ(appcode_texture.height, height);
  EXPECT_EQ(appcode_texture.target, GL_TEXTURE_2D);
  EXPECT_TRUE(release_callback_called);
}

TEST(appcodeWindowsTextureRegistrarTest, PopulateD3dTextureWithHandle) {
  std::unique_ptr<appcodeWindowsEngine> engine = GetTestEngine();
  auto gl = std::make_shared<egl::MockProcTable>();
  appcodeWindowsTextureRegistrar registrar(engine.get(), gl);

  UINT width = 100;
  UINT height = 100;
  auto d3d_texture = CreateD3dTexture(engine.get(), width, height);
  EXPECT_TRUE(d3d_texture);

  ComPtr<IDXGIResource> shared_resource;
  EXPECT_TRUE(SUCCEEDED(d3d_texture.As(&shared_resource)));

  HANDLE shared_handle;
  EXPECT_TRUE(SUCCEEDED(shared_resource->GetSharedHandle(&shared_handle)));

  bool release_callback_called = false;
  appcodeDesktopGpuSurfaceDescriptor surface_descriptor = {};
  surface_descriptor.struct_size = sizeof(appcodeDesktopGpuSurfaceDescriptor);
  surface_descriptor.handle = shared_handle;
  surface_descriptor.width = surface_descriptor.visible_width = width;
  surface_descriptor.height = surface_descriptor.visible_height = height;
  surface_descriptor.release_context = &release_callback_called;
  surface_descriptor.release_callback = [](void* release_context) {
    bool* called = reinterpret_cast<bool*>(release_context);
    *called = true;
  };

  appcodeDesktopTextureInfo texture_info = {};
  texture_info.type = kappcodeDesktopGpuSurfaceTexture;
  texture_info.gpu_surface_config.struct_size =
      sizeof(appcodeDesktopGpuSurfaceTextureConfig);
  texture_info.gpu_surface_config.type =
      kappcodeDesktopGpuSurfaceTypeDxgiSharedHandle;
  texture_info.gpu_surface_config.user_data = &surface_descriptor;
  texture_info.gpu_surface_config.callback =
      [](size_t width, size_t height,
         void* user_data) -> const appcodeDesktopGpuSurfaceDescriptor* {
    return reinterpret_cast<const appcodeDesktopGpuSurfaceDescriptor*>(
        user_data);
  };

  appcodeOpenGLTexture appcode_texture = {};
  auto texture_id = registrar.RegisterTexture(&texture_info);
  EXPECT_NE(texture_id, -1);

  EXPECT_CALL(*gl.get(), GenTextures(1, _))
      .Times(1)
      .WillOnce([](GLsizei n, GLuint* textures) { textures[0] = 1; });
  EXPECT_CALL(*gl.get(), BindTexture).Times(1);
  EXPECT_CALL(*gl.get(), TexParameteri).Times(AtLeast(1));
  EXPECT_CALL(*gl.get(), DeleteTextures(1, _)).Times(1);

  auto result =
      registrar.PopulateTexture(texture_id, 640, 480, &appcode_texture);
  EXPECT_TRUE(result);
  EXPECT_EQ(appcode_texture.width, width);
  EXPECT_EQ(appcode_texture.height, height);
  EXPECT_EQ(appcode_texture.target, GL_TEXTURE_2D);
  EXPECT_TRUE(release_callback_called);
}

TEST(appcodeWindowsTextureRegistrarTest, PopulateD3dTexture) {
  std::unique_ptr<appcodeWindowsEngine> engine = GetTestEngine();
  auto gl = std::make_shared<egl::MockProcTable>();
  appcodeWindowsTextureRegistrar registrar(engine.get(), gl);

  UINT width = 100;
  UINT height = 100;
  auto d3d_texture = CreateD3dTexture(engine.get(), width, height);
  EXPECT_TRUE(d3d_texture);

  bool release_callback_called = false;
  appcodeDesktopGpuSurfaceDescriptor surface_descriptor = {};
  surface_descriptor.struct_size = sizeof(appcodeDesktopGpuSurfaceDescriptor);
  surface_descriptor.handle = d3d_texture.Get();
  surface_descriptor.width = surface_descriptor.visible_width = width;
  surface_descriptor.height = surface_descriptor.visible_height = height;
  surface_descriptor.release_context = &release_callback_called;
  surface_descriptor.release_callback = [](void* release_context) {
    bool* called = reinterpret_cast<bool*>(release_context);
    *called = true;
  };

  appcodeDesktopTextureInfo texture_info = {};
  texture_info.type = kappcodeDesktopGpuSurfaceTexture;
  texture_info.gpu_surface_config.struct_size =
      sizeof(appcodeDesktopGpuSurfaceTextureConfig);
  texture_info.gpu_surface_config.type =
      kappcodeDesktopGpuSurfaceTypeD3d11Texture2D;
  texture_info.gpu_surface_config.user_data = &surface_descriptor;
  texture_info.gpu_surface_config.callback =
      [](size_t width, size_t height,
         void* user_data) -> const appcodeDesktopGpuSurfaceDescriptor* {
    return reinterpret_cast<const appcodeDesktopGpuSurfaceDescriptor*>(
        user_data);
  };

  appcodeOpenGLTexture appcode_texture = {};
  auto texture_id = registrar.RegisterTexture(&texture_info);
  EXPECT_NE(texture_id, -1);

  EXPECT_CALL(*gl.get(), GenTextures(1, _))
      .Times(1)
      .WillOnce([](GLsizei n, GLuint* textures) { textures[0] = 1; });
  EXPECT_CALL(*gl.get(), BindTexture).Times(1);
  EXPECT_CALL(*gl.get(), TexParameteri).Times(AtLeast(1));
  EXPECT_CALL(*gl.get(), DeleteTextures(1, _)).Times(1);

  auto result =
      registrar.PopulateTexture(texture_id, 640, 480, &appcode_texture);
  EXPECT_TRUE(result);
  EXPECT_EQ(appcode_texture.width, width);
  EXPECT_EQ(appcode_texture.height, height);
  EXPECT_EQ(appcode_texture.target, GL_TEXTURE_2D);
  EXPECT_TRUE(release_callback_called);
}

TEST(appcodeWindowsTextureRegistrarTest, PopulateInvalidTexture) {
  std::unique_ptr<appcodeWindowsEngine> engine = GetTestEngine();
  auto gl = std::make_shared<egl::MockProcTable>();

  appcodeWindowsTextureRegistrar registrar(engine.get(), gl);

  auto result = registrar.PopulateTexture(1, 640, 480, nullptr);
  EXPECT_FALSE(result);
}

TEST(appcodeWindowsTextureRegistrarTest,
     UnregisterTextureWithEngineDownInvokesCallback) {
  std::unique_ptr<appcodeWindowsEngine> engine = GetTestEngine();
  auto gl = std::make_shared<egl::MockProcTable>();

  appcodeWindowsTextureRegistrar registrar(engine.get(), gl);

  fml::AutoResetWaitableEvent latch;
  registrar.UnregisterTexture(1234, [&]() { latch.Signal(); });
  latch.Wait();
}

}  // namespace testing
}  // namespace appcode

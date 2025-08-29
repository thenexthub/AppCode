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

#include "appcode/shell/platform/embedder/tests/embedder_test_backingstore_producer_gl.h"

#include "appcode/fml/logging.h"
#include "appcode/testing/test_gl_surface.h"
#include "third_party/skia/include/core/SkColorSpace.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/SkSurfaceGanesh.h"
#include "third_party/skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/gl/GrGLTypes.h"

namespace appcode::testing {

namespace {
struct UserData {
  sk_sp<SkSurface> surface;
  std::unique_ptr<TestGLOnscreenOnlySurface> gl_surface;
};
}  // namespace

EmbedderTestBackingStoreProducerGL::EmbedderTestBackingStoreProducerGL(
    sk_sp<GrDirectContext> context,
    RenderTargetType type,
    std::shared_ptr<TestEGLContext> egl_context)
    : EmbedderTestBackingStoreProducer(std::move(context), type),
      test_egl_context_(std::move(egl_context)) {}

EmbedderTestBackingStoreProducerGL::~EmbedderTestBackingStoreProducerGL() =
    default;

bool EmbedderTestBackingStoreProducerGL::Create(
    const appcodeBackingStoreConfig* config,
    appcodeBackingStore* backing_store_out) {
  switch (type_) {
    case RenderTargetType::kOpenGLTexture:
      return CreateTexture(config, backing_store_out);
    case RenderTargetType::kOpenGLFramebuffer:
      return CreateFramebuffer(config, backing_store_out);
    case RenderTargetType::kOpenGLSurface:
      return CreateSurface(config, backing_store_out);
    default:
      return false;
  };
}

sk_sp<SkSurface> EmbedderTestBackingStoreProducerGL::GetSurface(
    const appcodeBackingStore* backing_store) const {
  UserData* user_data = reinterpret_cast<UserData*>(backing_store->user_data);
  return user_data->surface;
}

sk_sp<SkImage> EmbedderTestBackingStoreProducerGL::MakeImageSnapshot(
    const appcodeBackingStore* backing_store) const {
  UserData* user_data = reinterpret_cast<UserData*>(backing_store->user_data);
  if (user_data->gl_surface != nullptr) {
    // This backing store is an OpenGL Surface.
    // We need to make it current so we can snapshot it.
    user_data->gl_surface->MakeCurrent();

    // GetRasterSurfaceSnapshot() does two
    // gl_surface->makeImageSnapshot()'s. Doing a single
    // ->makeImageSnapshot() will not work.
    return user_data->gl_surface->GetRasterSurfaceSnapshot();
  }

  // Otherwise, it's a GL Texture or FrameBuffer.
  return user_data->surface->makeImageSnapshot();
}

bool EmbedderTestBackingStoreProducerGL::CreateFramebuffer(
    const appcodeBackingStoreConfig* config,
    appcodeBackingStore* backing_store_out) {
  const auto image_info =
      SkImageInfo::MakeN32Premul(config->size.width, config->size.height);

  auto surface =
      SkSurfaces::RenderTarget(context_.get(),               // context
                               skgpu::Budgeted::kNo,         // budgeted
                               image_info,                   // image info
                               1,                            // sample count
                               kBottomLeft_GrSurfaceOrigin,  // surface origin
                               nullptr,  // surface properties
                               false     // mipmaps
      );

  if (!surface) {
    FML_LOG(ERROR) << "Could not create render target for compositor layer.";
    return false;
  }

  GrBackendRenderTarget render_target = SkSurfaces::GetBackendRenderTarget(
      surface.get(), SkSurfaces::BackendHandleAccess::kDiscardWrite);

  if (!render_target.isValid()) {
    FML_LOG(ERROR) << "Backend render target was invalid.";
    return false;
  }

  GrGLFramebufferInfo framebuffer_info = {};
  if (!GrBackendRenderTargets::GetGLFramebufferInfo(render_target,
                                                    &framebuffer_info)) {
    FML_LOG(ERROR) << "Could not access backend framebuffer info.";
    return false;
  }

  auto user_data = new UserData{.surface = surface};
  backing_store_out->type = kappcodeBackingStoreTypeOpenGL;
  backing_store_out->user_data = user_data;
  backing_store_out->open_gl.type = kappcodeOpenGLTargetTypeFramebuffer;
  backing_store_out->open_gl.framebuffer.target = framebuffer_info.fFormat;
  backing_store_out->open_gl.framebuffer.name = framebuffer_info.fFBOID;
  backing_store_out->open_gl.framebuffer.user_data = user_data;
  backing_store_out->open_gl.framebuffer.destruction_callback =
      [](void* user_data) { delete reinterpret_cast<UserData*>(user_data); };

  return true;
}

bool EmbedderTestBackingStoreProducerGL::CreateTexture(
    const appcodeBackingStoreConfig* config,
    appcodeBackingStore* backing_store_out) {
  const auto image_info =
      SkImageInfo::MakeN32Premul(config->size.width, config->size.height);

  auto surface =
      SkSurfaces::RenderTarget(context_.get(),               // context
                               skgpu::Budgeted::kNo,         // budgeted
                               image_info,                   // image info
                               1,                            // sample count
                               kBottomLeft_GrSurfaceOrigin,  // surface origin
                               nullptr,  // surface properties
                               false     // mipmaps
      );

  if (!surface) {
    FML_LOG(ERROR) << "Could not create render target for compositor layer.";
    return false;
  }

  GrBackendTexture render_texture = SkSurfaces::GetBackendTexture(
      surface.get(), SkSurfaces::BackendHandleAccess::kDiscardWrite);

  if (!render_texture.isValid()) {
    FML_LOG(ERROR) << "Backend render texture was invalid.";
    return false;
  }

  GrGLTextureInfo texture_info = {};
  if (!GrBackendTextures::GetGLTextureInfo(render_texture, &texture_info)) {
    FML_LOG(ERROR) << "Could not access backend texture info.";
    return false;
  }

  auto user_data = new UserData{.surface = surface};
  backing_store_out->type = kappcodeBackingStoreTypeOpenGL;
  backing_store_out->user_data = user_data;
  backing_store_out->open_gl.type = kappcodeOpenGLTargetTypeTexture;
  backing_store_out->open_gl.texture.target = texture_info.fTarget;
  backing_store_out->open_gl.texture.name = texture_info.fID;
  backing_store_out->open_gl.texture.format = texture_info.fFormat;
  backing_store_out->open_gl.texture.user_data = user_data;
  backing_store_out->open_gl.texture.destruction_callback =
      [](void* user_data) { delete reinterpret_cast<UserData*>(user_data); };

  return true;
}

bool EmbedderTestBackingStoreProducerGL::CreateSurface(
    const appcodeBackingStoreConfig* config,
    appcodeBackingStore* backing_store_out) {
  FML_CHECK(test_egl_context_);
  auto surface = std::make_unique<TestGLOnscreenOnlySurface>(
      test_egl_context_,
      SkSize::Make(config->size.width, config->size.height).toRound());

  auto make_current = [](void* user_data, bool* invalidate_state) -> bool {
    *invalidate_state = false;
    return reinterpret_cast<UserData*>(user_data)->gl_surface->MakeCurrent();
  };

  auto clear_current = [](void* user_data, bool* invalidate_state) -> bool {
    *invalidate_state = false;
    // return
    // reinterpret_cast<GLUserData*>(user_data)->gl_surface->ClearCurrent();
    return true;
  };

  auto destruction_callback = [](void* user_data) {
    delete reinterpret_cast<UserData*>(user_data);
  };

  auto sk_surface = surface->GetOnscreenSurface();

  auto user_data = new UserData{
      .surface = sk_surface,
      .gl_surface = std::move(surface),
  };
  backing_store_out->type = kappcodeBackingStoreTypeOpenGL;
  backing_store_out->user_data = user_data;
  backing_store_out->open_gl.type = kappcodeOpenGLTargetTypeSurface;
  backing_store_out->open_gl.surface.user_data = user_data;
  backing_store_out->open_gl.surface.make_current_callback = make_current;
  backing_store_out->open_gl.surface.clear_current_callback = clear_current;
  backing_store_out->open_gl.surface.destruction_callback =
      destruction_callback;
  backing_store_out->open_gl.surface.format = 0x93A1 /* GL_BGRA8_EXT */;

  return true;
}

}  // namespace appcode::testing

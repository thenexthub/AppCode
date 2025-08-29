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
#define RAPIDJSON_HAS_STDSTRING 1

#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "appcode/fml/build_config.h"
#include "appcode/fml/closure.h"
#include "appcode/fml/make_copyable.h"
#include "appcode/fml/native_library.h"
#include "appcode/fml/thread.h"
#include "third_party/dart/runtime/bin/elf_loader.h"
#include "third_party/dart/runtime/include/dart_native_api.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/GpuTypes.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/SkSurfaceGanesh.h"

#if !defined(appcode_NO_EXPORT)
#if FML_OS_WIN
#define appcode_EXPORT __declspec(dllexport)
#else  // FML_OS_WIN
#define appcode_EXPORT __attribute__((visibility("default")))
#endif  // FML_OS_WIN
#endif  // !appcode_NO_EXPORT

extern "C" {
#if appcode_RUNTIME_MODE == appcode_RUNTIME_MODE_DEBUG
// Used for debugging dart:* sources.
extern const uint8_t kPlatformStrongDill[];
extern const intptr_t kPlatformStrongDillSize;
#endif  // appcode_RUNTIME_MODE == appcode_RUNTIME_MODE_DEBUG
}

#include "appcode/assets/directory_asset_bundle.h"
#include "appcode/common/graphics/persistent_cache.h"
#include "appcode/common/task_runners.h"
#include "appcode/fml/command_line.h"
#include "appcode/fml/file.h"
#include "appcode/fml/make_copyable.h"
#include "appcode/fml/message_loop.h"
#include "appcode/fml/paths.h"
#include "appcode/fml/trace_event.h"
#include "appcode/shell/common/rasterizer.h"
#include "appcode/shell/common/switches.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/embedder/embedder_engine.h"
#include "appcode/shell/platform/embedder/embedder_external_texture_resolver.h"
#include "appcode/shell/platform/embedder/embedder_platform_message_response.h"
#include "appcode/shell/platform/embedder/embedder_render_target.h"
#include "appcode/shell/platform/embedder/embedder_render_target_skia.h"
#include "appcode/shell/platform/embedder/embedder_semantics_update.h"
#include "appcode/shell/platform/embedder/embedder_struct_macros.h"
#include "appcode/shell/platform/embedder/embedder_task_runner.h"
#include "appcode/shell/platform/embedder/embedder_thread_host.h"
#include "appcode/shell/platform/embedder/pixel_formats.h"
#include "appcode/shell/platform/embedder/platform_view_embedder.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"

// Note: the IMPELLER_SUPPORTS_RENDERING may be defined even when the
// embedder/BUILD.gn variable impeller_supports_rendering is disabled.
#ifdef SHELL_ENABLE_GL
#include "appcode/shell/platform/embedder/embedder_external_texture_gl.h"
#include "third_party/skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/gl/GrGLTypes.h"
#ifdef IMPELLER_SUPPORTS_RENDERING
#include "appcode/shell/platform/embedder/embedder_render_target_impeller.h"  // nogncheck
#include "appcode/shell/platform/embedder/embedder_surface_gl_impeller.h"  // nogncheck
#include "appcode/shell/platform/embedder/embedder_surface_gl_skia.h"  // nogncheck
#include "impeller/core/texture.h"                        // nogncheck
#include "impeller/renderer/backend/gles/context_gles.h"  // nogncheck
#include "impeller/renderer/backend/gles/texture_gles.h"  // nogncheck
#include "impeller/renderer/context.h"                    // nogncheck
#include "impeller/renderer/render_target.h"              // nogncheck
#endif  // IMPELLER_SUPPORTS_RENDERING
#endif  // SHELL_ENABLE_GL

#ifdef SHELL_ENABLE_METAL
#include "appcode/shell/platform/embedder/embedder_surface_metal_skia.h"
#include "third_party/skia/include/gpu/ganesh/mtl/GrMtlBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/mtl/GrMtlTypes.h"
#include "third_party/skia/include/ports/SkCFObject.h"
#ifdef IMPELLER_SUPPORTS_RENDERING
#include "appcode/shell/platform/embedder/embedder_render_target_impeller.h"  // nogncheck
#include "appcode/shell/platform/embedder/embedder_surface_metal_impeller.h"  // nogncheck
#include "impeller/core/texture.h"                                // nogncheck
#include "impeller/renderer/backend/metal/texture_wrapper_mtl.h"  // nogncheck
#include "impeller/renderer/render_target.h"                      // nogncheck
#endif  // IMPELLER_SUPPORTS_RENDERING
#endif  // SHELL_ENABLE_METAL

#ifdef SHELL_ENABLE_VULKAN
#include "third_party/skia/include/gpu/ganesh/vk/GrVkBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/vk/GrVkTypes.h"
#endif  // SHELL_ENABLE_VULKAN

const int32_t kappcodeSemanticsNodeIdBatchEnd = -1;
const int32_t kappcodeSemanticsCustomActionIdBatchEnd = -1;

static constexpr appcodeViewId kappcodeImplicitViewId = 0;

// A message channel to send platform-independent appcodeKeyData to the
// framework.
//
// This should be kept in sync with the following variables:
//
// - lib/ui/platform_dispatcher.dart, _kappcodeKeyDataChannel
// - shell/platform/darwin/ios/framework/Source/appcodeEngine.mm,
//   appcodeKeyDataChannel
// - io/appcode/embedding/android/KeyData.java,
//   CHANNEL
//
// Not to be confused with "appcode/keyevent", which is used to send raw
// key event data in a platform-dependent format.
//
// ## Format
//
// Send: KeyDataPacket.data().
//
// Expected reply: Whether the event is handled. Exactly 1 byte long, with value
// 1 for handled, and 0 for not. Malformed value is considered false.
const char* kappcodeKeyDataChannel = "appcode/keydata";

static appcodeEngineResult LogEmbedderError(appcodeEngineResult code,
                                            const char* reason,
                                            const char* code_name,
                                            const char* function,
                                            const char* file,
                                            int line) {
#if FML_OS_WIN
  constexpr char kSeparator = '\\';
#else
  constexpr char kSeparator = '/';
#endif
  const auto file_base =
      (::strrchr(file, kSeparator) ? strrchr(file, kSeparator) + 1 : file);
  char error[256] = {};
  snprintf(error, (sizeof(error) / sizeof(char)),
           "%s (%d): '%s' returned '%s'. %s", file_base, line, function,
           code_name, reason);
  std::cerr << error << std::endl;
  return code;
}

#define LOG_EMBEDDER_ERROR(code, reason) \
  LogEmbedderError(code, reason, #code, __FUNCTION__, __FILE__, __LINE__)

static bool IsOpenGLRendererConfigValid(const appcodeRendererConfig* config) {
  if (config->type != kOpenGL) {
    return false;
  }

  const appcodeOpenGLRendererConfig* open_gl_config = &config->open_gl;

  if (!SAFE_EXISTS(open_gl_config, make_current) ||
      !SAFE_EXISTS(open_gl_config, clear_current) ||
      !SAFE_EXISTS_ONE_OF(open_gl_config, fbo_callback,
                          fbo_with_frame_info_callback) ||
      !SAFE_EXISTS_ONE_OF(open_gl_config, present, present_with_info)) {
    return false;
  }

  return true;
}

static bool IsSoftwareRendererConfigValid(const appcodeRendererConfig* config) {
  if (config->type != kSoftware) {
    return false;
  }

  const appcodeSoftwareRendererConfig* software_config = &config->software;

  if (SAFE_ACCESS(software_config, surface_present_callback, nullptr) ==
      nullptr) {
    return false;
  }

  return true;
}

static bool IsMetalRendererConfigValid(const appcodeRendererConfig* config) {
  if (config->type != kMetal) {
    return false;
  }

  const appcodeMetalRendererConfig* metal_config = &config->metal;

  bool device = SAFE_ACCESS(metal_config, device, nullptr);
  bool command_queue =
      SAFE_ACCESS(metal_config, present_command_queue, nullptr);

  bool present = SAFE_ACCESS(metal_config, present_drawable_callback, nullptr);
  bool get_texture =
      SAFE_ACCESS(metal_config, get_next_drawable_callback, nullptr);

  return device && command_queue && present && get_texture;
}

static bool IsVulkanRendererConfigValid(const appcodeRendererConfig* config) {
  if (config->type != kVulkan) {
    return false;
  }

  const appcodeVulkanRendererConfig* vulkan_config = &config->vulkan;

  if (!SAFE_EXISTS(vulkan_config, instance) ||
      !SAFE_EXISTS(vulkan_config, physical_device) ||
      !SAFE_EXISTS(vulkan_config, device) ||
      !SAFE_EXISTS(vulkan_config, queue) ||
      !SAFE_EXISTS(vulkan_config, get_instance_proc_address_callback) ||
      !SAFE_EXISTS(vulkan_config, get_next_image_callback) ||
      !SAFE_EXISTS(vulkan_config, present_image_callback)) {
    return false;
  }

  return true;
}

static bool IsRendererValid(const appcodeRendererConfig* config) {
  if (config == nullptr) {
    return false;
  }

  switch (config->type) {
    case kOpenGL:
      return IsOpenGLRendererConfigValid(config);
    case kSoftware:
      return IsSoftwareRendererConfigValid(config);
    case kMetal:
      return IsMetalRendererConfigValid(config);
    case kVulkan:
      return IsVulkanRendererConfigValid(config);
    default:
      return false;
  }

  return false;
}

#if FML_OS_LINUX || FML_OS_WIN
static void* DefaultGLProcResolver(const char* name) {
  static fml::RefPtr<fml::NativeLibrary> proc_library =
#if FML_OS_LINUX
      fml::NativeLibrary::CreateForCurrentProcess();
#elif FML_OS_WIN  // FML_OS_LINUX
      fml::NativeLibrary::Create("opengl32.dll");
#endif            // FML_OS_WIN
  return static_cast<void*>(
      const_cast<uint8_t*>(proc_library->ResolveSymbol(name)));
}
#endif  // FML_OS_LINUX || FML_OS_WIN

#ifdef SHELL_ENABLE_GL
// Auxiliary function used to translate rectangles of type SkIRect to
// appcodeRect.
static appcodeRect SkIRectToappcodeRect(const SkIRect sk_rect) {
  appcodeRect appcode_rect = {static_cast<double>(sk_rect.fLeft),
                              static_cast<double>(sk_rect.fTop),
                              static_cast<double>(sk_rect.fRight),
                              static_cast<double>(sk_rect.fBottom)};
  return appcode_rect;
}

// Auxiliary function used to translate rectangles of type appcodeRect to
// SkIRect.
static const SkIRect appcodeRectToSkIRect(appcodeRect appcode_rect) {
  SkIRect rect = {static_cast<int32_t>(appcode_rect.left),
                  static_cast<int32_t>(appcode_rect.top),
                  static_cast<int32_t>(appcode_rect.right),
                  static_cast<int32_t>(appcode_rect.bottom)};
  return rect;
}

// We need GL_BGRA8_EXT for creating SkSurfaces from appcodeOpenGLSurfaces
// below.
#ifndef GL_BGRA8_EXT
#define GL_BGRA8_EXT 0x93A1
#endif

static std::optional<SkColorType> appcodeFormatToSkColorType(uint32_t format) {
  switch (format) {
    case GL_BGRA8_EXT:
      return kBGRA_8888_SkColorType;
    case GL_RGBA8:
      return kRGBA_8888_SkColorType;
    default:
      FML_LOG(ERROR) << "Cannot convert format " << format
                     << " to SkColorType.";
      return std::nullopt;
  }
}

#endif

static inline appcode::Shell::CreateCallback<appcode::PlatformView>
InferOpenGLPlatformViewCreationCallback(
    const appcodeRendererConfig* config,
    void* user_data,
    const appcode::PlatformViewEmbedder::PlatformDispatchTable&
        platform_dispatch_table,
    std::unique_ptr<appcode::EmbedderExternalViewEmbedder>
        external_view_embedder,
    bool enable_impeller) {
#ifdef SHELL_ENABLE_GL
  if (config->type != kOpenGL) {
    return nullptr;
  }

  auto gl_make_current = [ptr = config->open_gl.make_current,
                          user_data]() -> bool { return ptr(user_data); };

  auto gl_clear_current = [ptr = config->open_gl.clear_current,
                           user_data]() -> bool { return ptr(user_data); };

  auto gl_present =
      [present = config->open_gl.present,
       present_with_info = config->open_gl.present_with_info,
       user_data](appcode::GLPresentInfo gl_present_info) -> bool {
    if (present) {
      return present(user_data);
    } else {
      // Format the frame and buffer damages accordingly. Note that, since the
      // current compute damage algorithm only returns one rectangle for damage
      // we are assuming the number of rectangles provided in frame and buffer
      // damage are always 1. Once the function that computes damage implements
      // support for multiple damage rectangles, GLPresentInfo should also
      // contain the number of damage rectangles.

      std::optional<appcodeRect> frame_damage_rect;
      if (gl_present_info.frame_damage) {
        frame_damage_rect =
            SkIRectToappcodeRect(*(gl_present_info.frame_damage));
      }
      std::optional<appcodeRect> buffer_damage_rect;
      if (gl_present_info.buffer_damage) {
        buffer_damage_rect =
            SkIRectToappcodeRect(*(gl_present_info.buffer_damage));
      }

      appcodeDamage frame_damage{
          .struct_size = sizeof(appcodeDamage),
          .num_rects = frame_damage_rect ? size_t{1} : size_t{0},
          .damage = frame_damage_rect ? &frame_damage_rect.value() : nullptr,
      };
      appcodeDamage buffer_damage{
          .struct_size = sizeof(appcodeDamage),
          .num_rects = buffer_damage_rect ? size_t{1} : size_t{0},
          .damage = buffer_damage_rect ? &buffer_damage_rect.value() : nullptr,
      };

      // Construct the present information concerning the frame being rendered.
      appcodePresentInfo present_info = {
          .struct_size = sizeof(appcodePresentInfo),
          .fbo_id = gl_present_info.fbo_id,
          .frame_damage = frame_damage,
          .buffer_damage = buffer_damage,
      };

      return present_with_info(user_data, &present_info);
    }
  };

  auto gl_fbo_callback =
      [fbo_callback = config->open_gl.fbo_callback,
       fbo_with_frame_info_callback =
           config->open_gl.fbo_with_frame_info_callback,
       user_data](appcode::GLFrameInfo gl_frame_info) -> intptr_t {
    if (fbo_callback) {
      return fbo_callback(user_data);
    } else {
      appcodeFrameInfo frame_info = {};
      frame_info.struct_size = sizeof(appcodeFrameInfo);
      frame_info.size = {gl_frame_info.width, gl_frame_info.height};
      return fbo_with_frame_info_callback(user_data, &frame_info);
    }
  };

  auto gl_populate_existing_damage =
      [populate_existing_damage = config->open_gl.populate_existing_damage,
       user_data](intptr_t id) -> appcode::GLFBOInfo {
    // If no populate_existing_damage was provided, disable partial
    // repaint.
    if (!populate_existing_damage) {
      return appcode::GLFBOInfo{
          .fbo_id = static_cast<uint32_t>(id),
          .existing_damage = std::nullopt,
      };
    }

    // Given the FBO's ID, get its existing damage.
    appcodeDamage existing_damage;
    populate_existing_damage(user_data, id, &existing_damage);

    std::optional<SkIRect> existing_damage_rect = std::nullopt;

    // Verify that at least one damage rectangle was provided.
    if (existing_damage.num_rects <= 0 || existing_damage.damage == nullptr) {
      FML_LOG(INFO) << "No damage was provided. Forcing full repaint.";
    } else {
      existing_damage_rect = SkIRect::MakeEmpty();
      for (size_t i = 0; i < existing_damage.num_rects; i++) {
        existing_damage_rect->join(
            appcodeRectToSkIRect(existing_damage.damage[i]));
      }
    }

    // Pass the information about this FBO to the rendering backend.
    return appcode::GLFBOInfo{
        .fbo_id = static_cast<uint32_t>(id),
        .existing_damage = existing_damage_rect,
    };
  };

  const appcodeOpenGLRendererConfig* open_gl_config = &config->open_gl;
  std::function<bool()> gl_make_resource_current_callback = nullptr;
  if (SAFE_ACCESS(open_gl_config, make_resource_current, nullptr) != nullptr) {
    gl_make_resource_current_callback =
        [ptr = config->open_gl.make_resource_current, user_data]() {
          return ptr(user_data);
        };
  }

  std::function<SkMatrix(void)> gl_surface_transformation_callback = nullptr;
  if (SAFE_ACCESS(open_gl_config, surface_transformation, nullptr) != nullptr) {
    gl_surface_transformation_callback =
        [ptr = config->open_gl.surface_transformation, user_data]() {
          appcodeTransformation transformation = ptr(user_data);
          return SkMatrix::MakeAll(transformation.scaleX,  //
                                   transformation.skewX,   //
                                   transformation.transX,  //
                                   transformation.skewY,   //
                                   transformation.scaleY,  //
                                   transformation.transY,  //
                                   transformation.pers0,   //
                                   transformation.pers1,   //
                                   transformation.pers2    //
          );
        };

    // If there is an external view embedder, ask it to apply the surface
    // transformation to its surfaces as well.
    if (external_view_embedder) {
      external_view_embedder->SetSurfaceTransformationCallback(
          gl_surface_transformation_callback);
    }
  }

  appcode::GPUSurfaceGLDelegate::GLProcResolver gl_proc_resolver = nullptr;
  if (SAFE_ACCESS(open_gl_config, gl_proc_resolver, nullptr) != nullptr) {
    gl_proc_resolver = [ptr = config->open_gl.gl_proc_resolver,
                        user_data](const char* gl_proc_name) {
      return ptr(user_data, gl_proc_name);
    };
  } else {
#if FML_OS_LINUX || FML_OS_WIN
    gl_proc_resolver = DefaultGLProcResolver;
#endif  // FML_OS_LINUX || FML_OS_WIN
  }

  bool fbo_reset_after_present =
      SAFE_ACCESS(open_gl_config, fbo_reset_after_present, false);

  appcode::EmbedderSurfaceGLSkia::GLDispatchTable gl_dispatch_table = {
      gl_make_current,                     // gl_make_current_callback
      gl_clear_current,                    // gl_clear_current_callback
      gl_present,                          // gl_present_callback
      gl_fbo_callback,                     // gl_fbo_callback
      gl_make_resource_current_callback,   // gl_make_resource_current_callback
      gl_surface_transformation_callback,  // gl_surface_transformation_callback
      gl_proc_resolver,                    // gl_proc_resolver
      gl_populate_existing_damage,         // gl_populate_existing_damage
  };

  return fml::MakeCopyable(
      [gl_dispatch_table, fbo_reset_after_present, platform_dispatch_table,
       enable_impeller,
       external_view_embedder =
           std::move(external_view_embedder)](appcode::Shell& shell) mutable {
        std::shared_ptr<appcode::EmbedderExternalViewEmbedder> view_embedder =
            std::move(external_view_embedder);
        if (enable_impeller) {
          return std::make_unique<appcode::PlatformViewEmbedder>(
              shell,                   // delegate
              shell.GetTaskRunners(),  // task runners
              std::make_unique<appcode::EmbedderSurfaceGLImpeller>(
                  gl_dispatch_table, fbo_reset_after_present,
                  view_embedder),       // embedder_surface
              platform_dispatch_table,  // embedder platform dispatch table
              view_embedder             // external view embedder
          );
        }
        return std::make_unique<appcode::PlatformViewEmbedder>(
            shell,                   // delegate
            shell.GetTaskRunners(),  // task runners
            std::make_unique<appcode::EmbedderSurfaceGLSkia>(
                gl_dispatch_table, fbo_reset_after_present,
                view_embedder),       // embedder_surface
            platform_dispatch_table,  // embedder platform dispatch table
            view_embedder             // external view embedder
        );
      });
#else   // SHELL_ENABLE_GL
  FML_LOG(ERROR) << "This appcode Engine does not support OpenGL rendering.";
  return nullptr;
#endif  // SHELL_ENABLE_GL
}

static appcode::Shell::CreateCallback<appcode::PlatformView>
InferMetalPlatformViewCreationCallback(
    const appcodeRendererConfig* config,
    void* user_data,
    const appcode::PlatformViewEmbedder::PlatformDispatchTable&
        platform_dispatch_table,
    std::unique_ptr<appcode::EmbedderExternalViewEmbedder>
        external_view_embedder,
    bool enable_impeller) {
  if (config->type != kMetal) {
    return nullptr;
  }

#ifdef SHELL_ENABLE_METAL
  std::function<bool(appcode::GPUMTLTextureInfo texture)> metal_present =
      [ptr = config->metal.present_drawable_callback,
       user_data](appcode::GPUMTLTextureInfo texture) {
        appcodeMetalTexture embedder_texture;
        embedder_texture.struct_size = sizeof(appcodeMetalTexture);
        embedder_texture.texture = texture.texture;
        embedder_texture.texture_id = texture.texture_id;
        embedder_texture.user_data = texture.destruction_context;
        embedder_texture.destruction_callback = texture.destruction_callback;
        return ptr(user_data, &embedder_texture);
      };
  auto metal_get_texture =
      [ptr = config->metal.get_next_drawable_callback,
       user_data](const SkISize& frame_size) -> appcode::GPUMTLTextureInfo {
    appcodeFrameInfo frame_info = {};
    frame_info.struct_size = sizeof(appcodeFrameInfo);
    frame_info.size = {static_cast<uint32_t>(frame_size.width()),
                       static_cast<uint32_t>(frame_size.height())};
    appcode::GPUMTLTextureInfo texture_info;

    appcodeMetalTexture metal_texture = ptr(user_data, &frame_info);
    texture_info.texture_id = metal_texture.texture_id;
    texture_info.texture = metal_texture.texture;
    texture_info.destruction_callback = metal_texture.destruction_callback;
    texture_info.destruction_context = metal_texture.user_data;
    return texture_info;
  };

  std::shared_ptr<appcode::EmbedderExternalViewEmbedder> view_embedder =
      std::move(external_view_embedder);

  std::unique_ptr<appcode::EmbedderSurface> embedder_surface;

  if (enable_impeller) {
    appcode::EmbedderSurfaceMetalImpeller::MetalDispatchTable
        metal_dispatch_table = {
            .present = metal_present,
            .get_texture = metal_get_texture,
        };
    embedder_surface = std::make_unique<appcode::EmbedderSurfaceMetalImpeller>(
        const_cast<appcode::GPUMTLDeviceHandle>(config->metal.device),
        const_cast<appcode::GPUMTLCommandQueueHandle>(
            config->metal.present_command_queue),
        metal_dispatch_table, view_embedder);
  } else {
#if !SLIMPELLER
    appcode::EmbedderSurfaceMetalSkia::MetalDispatchTable metal_dispatch_table =
        {
            .present = metal_present,
            .get_texture = metal_get_texture,
        };
    embedder_surface = std::make_unique<appcode::EmbedderSurfaceMetalSkia>(
        const_cast<appcode::GPUMTLDeviceHandle>(config->metal.device),
        const_cast<appcode::GPUMTLCommandQueueHandle>(
            config->metal.present_command_queue),
        metal_dispatch_table, view_embedder);
#else   //  !SLIMPELLER
    FML_LOG(FATAL) << "Impeller opt-out unavailable.";
#endif  //  !SLIMPELLER
  }

  // The static leak checker gets confused by the use of fml::MakeCopyable.
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
  return fml::MakeCopyable(
      [embedder_surface = std::move(embedder_surface), platform_dispatch_table,
       external_view_embedder = view_embedder](appcode::Shell& shell) mutable {
        return std::make_unique<appcode::PlatformViewEmbedder>(
            shell,                             // delegate
            shell.GetTaskRunners(),            // task runners
            std::move(embedder_surface),       // embedder surface
            platform_dispatch_table,           // platform dispatch table
            std::move(external_view_embedder)  // external view embedder
        );
      });
#else   // SHELL_ENABLE_METAL
  FML_LOG(ERROR) << "This appcode Engine does not support Metal rendering.";
  return nullptr;
#endif  // SHELL_ENABLE_METAL
}

static appcode::Shell::CreateCallback<appcode::PlatformView>
InferVulkanPlatformViewCreationCallback(
    const appcodeRendererConfig* config,
    void* user_data,
    const appcode::PlatformViewEmbedder::PlatformDispatchTable&
        platform_dispatch_table,
    std::unique_ptr<appcode::EmbedderExternalViewEmbedder>
        external_view_embedder,
    bool enable_impeller) {
  if (config->type != kVulkan) {
    return nullptr;
  }

#ifdef SHELL_ENABLE_VULKAN
  std::function<void*(VkInstance, const char*)>
      vulkan_get_instance_proc_address =
          [ptr = config->vulkan.get_instance_proc_address_callback, user_data](
              VkInstance instance, const char* proc_name) -> void* {
    return ptr(user_data, instance, proc_name);
  };

  auto vulkan_get_next_image =
      [ptr = config->vulkan.get_next_image_callback,
       user_data](const SkISize& frame_size) -> appcodeVulkanImage {
    appcodeFrameInfo frame_info = {
        .struct_size = sizeof(appcodeFrameInfo),
        .size = {static_cast<uint32_t>(frame_size.width()),
                 static_cast<uint32_t>(frame_size.height())},
    };

    return ptr(user_data, &frame_info);
  };

  auto vulkan_present_image_callback =
      [ptr = config->vulkan.present_image_callback, user_data](
          VkImage image, VkFormat format) -> bool {
    appcodeVulkanImage image_desc = {
        .struct_size = sizeof(appcodeVulkanImage),
        .image = reinterpret_cast<uint64_t>(image),
        .format = static_cast<uint32_t>(format),
    };
    return ptr(user_data, &image_desc);
  };

  auto vk_instance = static_cast<VkInstance>(config->vulkan.instance);
  auto proc_addr =
      vulkan_get_instance_proc_address(vk_instance, "vkGetInstanceProcAddr");

  std::shared_ptr<appcode::EmbedderExternalViewEmbedder> view_embedder =
      std::move(external_view_embedder);

#if IMPELLER_SUPPORTS_RENDERING
  if (enable_impeller) {
    appcode::EmbedderSurfaceVulkanImpeller::VulkanDispatchTable
        vulkan_dispatch_table = {
            .get_instance_proc_address =
                reinterpret_cast<PFN_vkGetInstanceProcAddr>(proc_addr),
            .get_next_image = vulkan_get_next_image,
            .present_image = vulkan_present_image_callback,
        };

    std::unique_ptr<appcode::EmbedderSurfaceVulkanImpeller> embedder_surface =
        std::make_unique<appcode::EmbedderSurfaceVulkanImpeller>(
            config->vulkan.version, vk_instance,
            config->vulkan.enabled_instance_extension_count,
            config->vulkan.enabled_instance_extensions,
            config->vulkan.enabled_device_extension_count,
            config->vulkan.enabled_device_extensions,
            static_cast<VkPhysicalDevice>(config->vulkan.physical_device),
            static_cast<VkDevice>(config->vulkan.device),
            config->vulkan.queue_family_index,
            static_cast<VkQueue>(config->vulkan.queue), vulkan_dispatch_table,
            view_embedder);

    return fml::MakeCopyable(
        [embedder_surface = std::move(embedder_surface),
         platform_dispatch_table,
         external_view_embedder =
             std::move(view_embedder)](appcode::Shell& shell) mutable {
          return std::make_unique<appcode::PlatformViewEmbedder>(
              shell,                             // delegate
              shell.GetTaskRunners(),            // task runners
              std::move(embedder_surface),       // embedder surface
              platform_dispatch_table,           // platform dispatch table
              std::move(external_view_embedder)  // external view embedder
          );
        });
  } else {
    appcode::EmbedderSurfaceVulkan::VulkanDispatchTable vulkan_dispatch_table =
        {
            .get_instance_proc_address =
                reinterpret_cast<PFN_vkGetInstanceProcAddr>(proc_addr),
            .get_next_image = vulkan_get_next_image,
            .present_image = vulkan_present_image_callback,
        };

    std::unique_ptr<appcode::EmbedderSurfaceVulkan> embedder_surface =
        std::make_unique<appcode::EmbedderSurfaceVulkan>(
            config->vulkan.version, vk_instance,
            config->vulkan.enabled_instance_extension_count,
            config->vulkan.enabled_instance_extensions,
            config->vulkan.enabled_device_extension_count,
            config->vulkan.enabled_device_extensions,
            static_cast<VkPhysicalDevice>(config->vulkan.physical_device),
            static_cast<VkDevice>(config->vulkan.device),
            config->vulkan.queue_family_index,
            static_cast<VkQueue>(config->vulkan.queue), vulkan_dispatch_table,
            view_embedder);

    return fml::MakeCopyable(
        [embedder_surface = std::move(embedder_surface),
         platform_dispatch_table,
         external_view_embedder =
             std::move(view_embedder)](appcode::Shell& shell) mutable {
          return std::make_unique<appcode::PlatformViewEmbedder>(
              shell,                             // delegate
              shell.GetTaskRunners(),            // task runners
              std::move(embedder_surface),       // embedder surface
              platform_dispatch_table,           // platform dispatch table
              std::move(external_view_embedder)  // external view embedder
          );
        });
  }
#else
  appcode::EmbedderSurfaceVulkan::VulkanDispatchTable vulkan_dispatch_table = {
      .get_instance_proc_address =
          reinterpret_cast<PFN_vkGetInstanceProcAddr>(proc_addr),
      .get_next_image = vulkan_get_next_image,
      .present_image = vulkan_present_image_callback,
  };

  std::unique_ptr<appcode::EmbedderSurfaceVulkan> embedder_surface =
      std::make_unique<appcode::EmbedderSurfaceVulkan>(
          config->vulkan.version, vk_instance,
          config->vulkan.enabled_instance_extension_count,
          config->vulkan.enabled_instance_extensions,
          config->vulkan.enabled_device_extension_count,
          config->vulkan.enabled_device_extensions,
          static_cast<VkPhysicalDevice>(config->vulkan.physical_device),
          static_cast<VkDevice>(config->vulkan.device),
          config->vulkan.queue_family_index,
          static_cast<VkQueue>(config->vulkan.queue), vulkan_dispatch_table,
          view_embedder);

  return fml::MakeCopyable(
      [embedder_surface = std::move(embedder_surface), platform_dispatch_table,
       external_view_embedder =
           std::move(view_embedder)](appcode::Shell& shell) mutable {
        return std::make_unique<appcode::PlatformViewEmbedder>(
            shell,                             // delegate
            shell.GetTaskRunners(),            // task runners
            std::move(embedder_surface),       // embedder surface
            platform_dispatch_table,           // platform dispatch table
            std::move(external_view_embedder)  // external view embedder
        );
      });
#endif  //  // IMPELLER_SUPPORTS_RENDERING
#else   // SHELL_ENABLE_VULKAN
  FML_LOG(ERROR) << "This appcode Engine does not support Vulkan rendering.";
  return nullptr;
#endif  // SHELL_ENABLE_VULKAN
}

static appcode::Shell::CreateCallback<appcode::PlatformView>
InferSoftwarePlatformViewCreationCallback(
    const appcodeRendererConfig* config,
    void* user_data,
    const appcode::PlatformViewEmbedder::PlatformDispatchTable&
        platform_dispatch_table,
    std::unique_ptr<appcode::EmbedderExternalViewEmbedder>
        external_view_embedder) {
  if (config->type != kSoftware) {
    return nullptr;
  }

  auto software_present_backing_store =
      [ptr = config->software.surface_present_callback, user_data](
          const void* allocation, size_t row_bytes, size_t height) -> bool {
    return ptr(user_data, allocation, row_bytes, height);
  };

  appcode::EmbedderSurfaceSoftware::SoftwareDispatchTable
      software_dispatch_table = {
          software_present_backing_store,  // required
      };

  return fml::MakeCopyable(
      [software_dispatch_table, platform_dispatch_table,
       external_view_embedder =
           std::move(external_view_embedder)](appcode::Shell& shell) mutable {
        return std::make_unique<appcode::PlatformViewEmbedder>(
            shell,                             // delegate
            shell.GetTaskRunners(),            // task runners
            software_dispatch_table,           // software dispatch table
            platform_dispatch_table,           // platform dispatch table
            std::move(external_view_embedder)  // external view embedder
        );
      });
}

static appcode::Shell::CreateCallback<appcode::PlatformView>
InferPlatformViewCreationCallback(
    const appcodeRendererConfig* config,
    void* user_data,
    const appcode::PlatformViewEmbedder::PlatformDispatchTable&
        platform_dispatch_table,
    std::unique_ptr<appcode::EmbedderExternalViewEmbedder>
        external_view_embedder,
    bool enable_impeller) {
  if (config == nullptr) {
    return nullptr;
  }

  switch (config->type) {
    case kOpenGL:
      return InferOpenGLPlatformViewCreationCallback(
          config, user_data, platform_dispatch_table,
          std::move(external_view_embedder), enable_impeller);
    case kSoftware:
      return InferSoftwarePlatformViewCreationCallback(
          config, user_data, platform_dispatch_table,
          std::move(external_view_embedder));
    case kMetal:
      return InferMetalPlatformViewCreationCallback(
          config, user_data, platform_dispatch_table,
          std::move(external_view_embedder), enable_impeller);
    case kVulkan:
      return InferVulkanPlatformViewCreationCallback(
          config, user_data, platform_dispatch_table,
          std::move(external_view_embedder), enable_impeller);
    default:
      return nullptr;
  }
  return nullptr;
}

static sk_sp<SkSurface> MakeSkSurfaceFromBackingStore(
    GrDirectContext* context,
    const appcodeBackingStoreConfig& config,
    const appcodeOpenGLTexture* texture) {
#ifdef SHELL_ENABLE_GL
  GrGLTextureInfo texture_info;
  texture_info.fTarget = texture->target;
  texture_info.fID = texture->name;
  texture_info.fFormat = texture->format;

  GrBackendTexture backend_texture =
      GrBackendTextures::MakeGL(config.size.width, config.size.height,
                                skgpu::Mipmapped::kNo, texture_info);

  SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

  std::optional<SkColorType> color_type =
      appcodeFormatToSkColorType(texture->format);
  if (!color_type) {
    return nullptr;
  }

  auto surface = SkSurfaces::WrapBackendTexture(
      context,                      // context
      backend_texture,              // back-end texture
      kBottomLeft_GrSurfaceOrigin,  // surface origin
      1,                            // sample count
      color_type.value(),           // color type
      SkColorSpace::MakeSRGB(),     // color space
      &surface_properties,          // surface properties
      static_cast<SkSurfaces::TextureReleaseProc>(
          texture->destruction_callback),  // release proc
      texture->user_data                   // release context
  );

  if (!surface) {
    FML_LOG(ERROR) << "Could not wrap embedder supplied render texture.";
    return nullptr;
  }

  return surface;
#else
  return nullptr;
#endif
}

static sk_sp<SkSurface> MakeSkSurfaceFromBackingStore(
    GrDirectContext* context,
    const appcodeBackingStoreConfig& config,
    const appcodeOpenGLFramebuffer* framebuffer) {
#ifdef SHELL_ENABLE_GL
  GrGLFramebufferInfo framebuffer_info = {};
  framebuffer_info.fFormat = framebuffer->target;
  framebuffer_info.fFBOID = framebuffer->name;

  auto backend_render_target =
      GrBackendRenderTargets::MakeGL(config.size.width,   // width
                                     config.size.height,  // height
                                     1,                   // sample count
                                     0,                   // stencil bits
                                     framebuffer_info     // framebuffer info
      );

  SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

  std::optional<SkColorType> color_type =
      appcodeFormatToSkColorType(framebuffer->target);
  if (!color_type) {
    return nullptr;
  }

  auto surface = SkSurfaces::WrapBackendRenderTarget(
      context,                      //  context
      backend_render_target,        // backend render target
      kBottomLeft_GrSurfaceOrigin,  // surface origin
      color_type.value(),           // color type
      SkColorSpace::MakeSRGB(),     // color space
      &surface_properties,          // surface properties
      static_cast<SkSurfaces::RenderTargetReleaseProc>(
          framebuffer->destruction_callback),  // release proc
      framebuffer->user_data                   // release context
  );

  if (!surface) {
    FML_LOG(ERROR) << "Could not wrap embedder supplied frame-buffer.";
    return nullptr;
  }
  return surface;
#else
  return nullptr;
#endif
}

static sk_sp<SkSurface> MakeSkSurfaceFromBackingStore(
    GrDirectContext* context,
    const appcodeBackingStoreConfig& config,
    const appcodeOpenGLSurface* surface) {
#ifdef SHELL_ENABLE_GL
  GrGLFramebufferInfo framebuffer_info = {};
  framebuffer_info.fFormat = SAFE_ACCESS(surface, format, GL_BGRA8_EXT);
  framebuffer_info.fFBOID = 0;

  auto backend_render_target =
      GrBackendRenderTargets::MakeGL(config.size.width,   // width
                                     config.size.height,  // height
                                     1,                   // sample count
                                     0,                   // stencil bits
                                     framebuffer_info     // framebuffer info
      );

  SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

  std::optional<SkColorType> color_type =
      appcodeFormatToSkColorType(surface->format);
  if (!color_type) {
    return nullptr;
  }

  auto sk_surface = SkSurfaces::WrapBackendRenderTarget(
      context,                      //  context
      backend_render_target,        // backend render target
      kBottomLeft_GrSurfaceOrigin,  // surface origin
      color_type.value(),           // color type
      SkColorSpace::MakeSRGB(),     // color space
      &surface_properties,          // surface properties
      static_cast<SkSurfaces::RenderTargetReleaseProc>(
          surface->destruction_callback),  // release proc
      surface->user_data                   // release context
  );

  if (!sk_surface) {
    FML_LOG(ERROR) << "Could not wrap embedder supplied frame-buffer.";
    return nullptr;
  }
  return sk_surface;
#else
  return nullptr;
#endif
}

static sk_sp<SkSurface> MakeSkSurfaceFromBackingStore(
    GrDirectContext* context,
    const appcodeBackingStoreConfig& config,
    const appcodeSoftwareBackingStore* software) {
  const auto image_info =
      SkImageInfo::MakeN32Premul(config.size.width, config.size.height);

  struct Captures {
    VoidCallback destruction_callback;
    void* user_data;
  };
  auto captures = std::make_unique<Captures>();
  captures->destruction_callback = software->destruction_callback;
  captures->user_data = software->user_data;
  auto release_proc = [](void* pixels, void* context) {
    auto captures = reinterpret_cast<Captures*>(context);
    if (captures->destruction_callback) {
      captures->destruction_callback(captures->user_data);
    }
    delete captures;
  };

  auto surface =
      SkSurfaces::WrapPixels(image_info,  // image info
                             const_cast<void*>(software->allocation),  // pixels
                             software->row_bytes,  // row bytes
                             release_proc,         // release proc
                             captures.get()        // get context
      );

  if (!surface) {
    FML_LOG(ERROR)
        << "Could not wrap embedder supplied software render buffer.";
    if (software->destruction_callback) {
      software->destruction_callback(software->user_data);
    }
    return nullptr;
  }
  if (surface) {
    captures.release();  // Skia has assumed ownership of the struct.
  }
  return surface;
}

static sk_sp<SkSurface> MakeSkSurfaceFromBackingStore(
    GrDirectContext* context,
    const appcodeBackingStoreConfig& config,
    const appcodeSoftwareBackingStore2* software) {
  const auto color_info = getSkColorInfo(software->pixel_format);
  if (!color_info) {
    return nullptr;
  }

  const auto image_info = SkImageInfo::Make(
      SkISize::Make(config.size.width, config.size.height), *color_info);

  struct Captures {
    VoidCallback destruction_callback;
    void* user_data;
  };
  auto captures = std::make_unique<Captures>();
  captures->destruction_callback = software->destruction_callback;
  captures->user_data = software->user_data;
  auto release_proc = [](void* pixels, void* context) {
    auto captures = reinterpret_cast<Captures*>(context);
    if (captures->destruction_callback) {
      captures->destruction_callback(captures->user_data);
    }
  };

  auto surface =
      SkSurfaces::WrapPixels(image_info,  // image info
                             const_cast<void*>(software->allocation),  // pixels
                             software->row_bytes,  // row bytes
                             release_proc,         // release proc
                             captures.release()    // release context
      );

  if (!surface) {
    FML_LOG(ERROR)
        << "Could not wrap embedder supplied software render buffer.";
    if (software->destruction_callback) {
      software->destruction_callback(software->user_data);
    }
    return nullptr;
  }
  return surface;
}

static sk_sp<SkSurface> MakeSkSurfaceFromBackingStore(
    GrDirectContext* context,
    const appcodeBackingStoreConfig& config,
    const appcodeMetalBackingStore* metal) {
#if defined(SHELL_ENABLE_METAL) && !SLIMPELLER
  GrMtlTextureInfo texture_info;
  if (!metal->texture.texture) {
    FML_LOG(ERROR) << "Embedder supplied null Metal texture.";
    return nullptr;
  }
  sk_cfp<appcodeMetalTextureHandle> mtl_texture;
  mtl_texture.retain(metal->texture.texture);
  texture_info.fTexture = mtl_texture;
  GrBackendTexture backend_texture =
      GrBackendTextures::MakeMtl(config.size.width,      //
                                 config.size.height,     //
                                 skgpu::Mipmapped::kNo,  //
                                 texture_info            //
      );

  SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

  auto surface = SkSurfaces::WrapBackendTexture(
      context,                   // context
      backend_texture,           // back-end texture
      kTopLeft_GrSurfaceOrigin,  // surface origin
      1,                         // sample count
      kBGRA_8888_SkColorType,    // color type
      nullptr,                   // color space
      &surface_properties,       // surface properties
      static_cast<SkSurfaces::TextureReleaseProc>(
          metal->texture.destruction_callback),  // release proc
      metal->texture.user_data                   // release context
  );

  if (!surface) {
    FML_LOG(ERROR) << "Could not wrap embedder supplied Metal render texture.";
    return nullptr;
  }

  return surface;
#else
  return nullptr;
#endif
}

#if defined(SHELL_ENABLE_GL) && defined(IMPELLER_SUPPORTS_RENDERING)
static std::optional<impeller::PixelFormat> appcodeFormatToImpellerPixelFormat(
    uint32_t format) {
  switch (format) {
    case GL_BGRA8_EXT:
      return impeller::PixelFormat::kB8G8R8A8UNormInt;
    case GL_RGBA8:
      return impeller::PixelFormat::kR8G8B8A8UNormInt;
    default:
      FML_LOG(ERROR) << "Cannot convert format " << format
                     << " to impeller::PixelFormat.";
      return std::nullopt;
  }
}

#endif  // defined(SHELL_ENABLE_GL) && defined(IMPELLER_SUPPORTS_RENDERING)

static std::unique_ptr<appcode::EmbedderRenderTarget>
MakeRenderTargetFromBackingStoreImpeller(
    appcodeBackingStore backing_store,
    const fml::closure& on_release,
    const std::shared_ptr<impeller::AiksContext>& aiks_context,
    const appcodeBackingStoreConfig& config,
    const appcodeOpenGLFramebuffer* framebuffer) {
#if defined(SHELL_ENABLE_GL) && defined(IMPELLER_SUPPORTS_RENDERING)
  auto format = appcodeFormatToImpellerPixelFormat(framebuffer->target);
  if (!format.has_value()) {
    return nullptr;
  }

  const auto& gl_context =
      impeller::ContextGLES::Cast(*aiks_context->GetContext());
  const bool implicit_msaa = aiks_context->GetContext()
                                 ->GetCapabilities()
                                 ->SupportsImplicitResolvingMSAA();
  const auto size = impeller::ISize(config.size.width, config.size.height);

  impeller::TextureDescriptor color0_tex;
  if (implicit_msaa) {
    color0_tex.type = impeller::TextureType::kTexture2DMultisample;
    color0_tex.sample_count = impeller::SampleCount::kCount4;
  } else {
    color0_tex.type = impeller::TextureType::kTexture2D;
    color0_tex.sample_count = impeller::SampleCount::kCount1;
  }
  color0_tex.format = format.value();
  color0_tex.size = size;
  color0_tex.usage = static_cast<impeller::TextureUsageMask>(
      impeller::TextureUsage::kRenderTarget);
  color0_tex.storage_mode = impeller::StorageMode::kDevicePrivate;

  impeller::ColorAttachment color0;
  color0.texture = impeller::TextureGLES::WrapFBO(
      gl_context.GetReactor(), color0_tex, framebuffer->name);
  color0.clear_color = impeller::Color::DarkSlateGray();
  color0.load_action = impeller::LoadAction::kClear;
  if (implicit_msaa) {
    color0.store_action = impeller::StoreAction::kMultisampleResolve;
    color0.resolve_texture = color0.texture;
  } else {
    color0.store_action = impeller::StoreAction::kStore;
  }

  impeller::TextureDescriptor depth_stencil_texture_desc;
  depth_stencil_texture_desc.format = impeller::PixelFormat::kD24UnormS8Uint;
  depth_stencil_texture_desc.size = size;
  depth_stencil_texture_desc.usage = static_cast<impeller::TextureUsageMask>(
      impeller::TextureUsage::kRenderTarget);
  if (implicit_msaa) {
    depth_stencil_texture_desc.type =
        impeller::TextureType::kTexture2DMultisample;
    depth_stencil_texture_desc.sample_count = impeller::SampleCount::kCount4;
  } else {
    depth_stencil_texture_desc.type = impeller::TextureType::kTexture2D;
    depth_stencil_texture_desc.sample_count = impeller::SampleCount::kCount1;
  }

  auto depth_stencil_tex = impeller::TextureGLES::CreatePlaceholder(
      gl_context.GetReactor(), depth_stencil_texture_desc);

  impeller::DepthAttachment depth0;
  depth0.clear_depth = 0;
  depth0.texture = depth_stencil_tex;
  depth0.load_action = impeller::LoadAction::kClear;
  depth0.store_action = impeller::StoreAction::kDontCare;

  impeller::StencilAttachment stencil0;
  stencil0.clear_stencil = 0;
  stencil0.texture = depth_stencil_tex;
  stencil0.load_action = impeller::LoadAction::kClear;
  stencil0.store_action = impeller::StoreAction::kDontCare;

  impeller::RenderTarget render_target_desc;

  render_target_desc.SetColorAttachment(color0, 0u);
  render_target_desc.SetDepthAttachment(depth0);
  render_target_desc.SetStencilAttachment(stencil0);

  fml::closure framebuffer_destruct =
      [callback = framebuffer->destruction_callback,
       user_data = framebuffer->user_data]() { callback(user_data); };

  return std::make_unique<appcode::EmbedderRenderTargetImpeller>(
      backing_store, aiks_context,
      std::make_unique<impeller::RenderTarget>(std::move(render_target_desc)),
      on_release, framebuffer_destruct);
#else
  return nullptr;
#endif
}

static std::unique_ptr<appcode::EmbedderRenderTarget>
MakeRenderTargetFromBackingStoreImpeller(
    appcodeBackingStore backing_store,
    const fml::closure& on_release,
    const std::shared_ptr<impeller::AiksContext>& aiks_context,
    const appcodeBackingStoreConfig& config,
    const appcodeMetalBackingStore* metal) {
#if defined(SHELL_ENABLE_METAL) && defined(IMPELLER_SUPPORTS_RENDERING)
  if (!metal->texture.texture) {
    FML_LOG(ERROR) << "Embedder supplied null Metal texture.";
    return nullptr;
  }

  const auto size = impeller::ISize(config.size.width, config.size.height);

  impeller::TextureDescriptor resolve_tex_desc;
  resolve_tex_desc.size = size;
  resolve_tex_desc.sample_count = impeller::SampleCount::kCount1;
  resolve_tex_desc.storage_mode = impeller::StorageMode::kDevicePrivate;
  resolve_tex_desc.usage = impeller::TextureUsage::kRenderTarget |
                           impeller::TextureUsage::kShaderRead;

  auto resolve_tex = impeller::WrapTextureMTL(
      resolve_tex_desc, metal->texture.texture,
      [callback = metal->texture.destruction_callback,
       user_data = metal->texture.user_data]() { callback(user_data); });
  if (!resolve_tex) {
    FML_LOG(ERROR) << "Could not wrap embedder supplied Metal render texture.";
    return nullptr;
  }
  resolve_tex->SetLabel("ImpellerBackingStoreResolve");

  impeller::TextureDescriptor msaa_tex_desc;
  msaa_tex_desc.storage_mode = impeller::StorageMode::kDeviceTransient;
  msaa_tex_desc.type = impeller::TextureType::kTexture2DMultisample;
  msaa_tex_desc.sample_count = impeller::SampleCount::kCount4;
  msaa_tex_desc.format = resolve_tex->GetTextureDescriptor().format;
  msaa_tex_desc.size = size;
  msaa_tex_desc.usage = impeller::TextureUsage::kRenderTarget;

  auto msaa_tex =
      aiks_context->GetContext()->GetResourceAllocator()->CreateTexture(
          msaa_tex_desc);
  if (!msaa_tex) {
    FML_LOG(ERROR) << "Could not allocate MSAA color texture.";
    return nullptr;
  }
  msaa_tex->SetLabel("ImpellerBackingStoreColorMSAA");

  impeller::ColorAttachment color0;
  color0.texture = msaa_tex;
  color0.clear_color = impeller::Color::DarkSlateGray();
  color0.load_action = impeller::LoadAction::kClear;
  color0.store_action = impeller::StoreAction::kMultisampleResolve;
  color0.resolve_texture = resolve_tex;

  impeller::RenderTarget render_target_desc;
  render_target_desc.SetColorAttachment(color0, 0u);

  return std::make_unique<appcode::EmbedderRenderTargetImpeller>(
      backing_store, aiks_context,
      std::make_unique<impeller::RenderTarget>(std::move(render_target_desc)),
      on_release, fml::closure());
#else
  return nullptr;
#endif
}

static sk_sp<SkSurface> MakeSkSurfaceFromBackingStore(
    GrDirectContext* context,
    const appcodeBackingStoreConfig& config,
    const appcodeVulkanBackingStore* vulkan) {
#ifdef SHELL_ENABLE_VULKAN
  if (!vulkan->image) {
    FML_LOG(ERROR) << "Embedder supplied null Vulkan image.";
    return nullptr;
  }
  GrVkImageInfo image_info = {
      .fImage = reinterpret_cast<VkImage>(vulkan->image->image),
      .fImageTiling = VK_IMAGE_TILING_OPTIMAL,
      .fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .fFormat = static_cast<VkFormat>(vulkan->image->format),
      .fImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                          VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                          VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                          VK_IMAGE_USAGE_SAMPLED_BIT,
      .fSampleCount = 1,
      .fLevelCount = 1,
  };
  auto backend_texture = GrBackendTextures::MakeVk(
      config.size.width, config.size.height, image_info);

  SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

  auto surface = SkSurfaces::WrapBackendTexture(
      context,                   // context
      backend_texture,           // back-end texture
      kTopLeft_GrSurfaceOrigin,  // surface origin
      1,                         // sample count
      appcode::GPUSurfaceVulkan::ColorTypeFromFormat(
          static_cast<VkFormat>(vulkan->image->format)),  // color type
      SkColorSpace::MakeSRGB(),                           // color space
      &surface_properties,                                // surface properties
      static_cast<SkSurfaces::TextureReleaseProc>(
          vulkan->destruction_callback),  // release proc
      vulkan->user_data                   // release context
  );

  if (!surface) {
    FML_LOG(ERROR) << "Could not wrap embedder supplied Vulkan render texture.";
    return nullptr;
  }

  return surface;
#else
  return nullptr;
#endif
}

static std::unique_ptr<appcode::EmbedderRenderTarget>
MakeRenderTargetFromSkSurface(
    appcodeBackingStore backing_store,
    sk_sp<SkSurface> skia_surface,
    fml::closure on_release,
    appcode::EmbedderRenderTarget::MakeOrClearCurrentCallback on_make_current,
    appcode::EmbedderRenderTarget::MakeOrClearCurrentCallback
        on_clear_current) {
  if (!skia_surface) {
    return nullptr;
  }
  return std::make_unique<appcode::EmbedderRenderTargetSkia>(
      backing_store, std::move(skia_surface), std::move(on_release),
      std::move(on_make_current), std::move(on_clear_current));
}

static std::unique_ptr<appcode::EmbedderRenderTarget>
MakeRenderTargetFromSkSurface(appcodeBackingStore backing_store,
                              sk_sp<SkSurface> skia_surface,
                              fml::closure on_release) {
  return MakeRenderTargetFromSkSurface(backing_store, std::move(skia_surface),
                                       std::move(on_release), nullptr, nullptr);
}

static std::unique_ptr<appcode::EmbedderRenderTarget>
CreateEmbedderRenderTarget(
    const appcodeCompositor* compositor,
    const appcodeBackingStoreConfig& config,
    GrDirectContext* context,
    const std::shared_ptr<impeller::AiksContext>& aiks_context,
    bool enable_impeller) {
  appcodeBackingStore backing_store = {};
  backing_store.struct_size = sizeof(backing_store);

  // Safe access checks on the compositor struct have been performed in
  // InferExternalViewEmbedderFromArgs and are not necessary here.
  auto c_create_callback = compositor->create_backing_store_callback;
  auto c_collect_callback = compositor->collect_backing_store_callback;

  {
    TRACE_EVENT0("appcode", "appcodeCompositorCreateBackingStore");
    if (!c_create_callback(&config, &backing_store, compositor->user_data)) {
      FML_LOG(ERROR) << "Could not create the embedder backing store.";
      return nullptr;
    }
  }

  if (backing_store.struct_size != sizeof(backing_store)) {
    FML_LOG(ERROR) << "Embedder modified the backing store struct size.";
    return nullptr;
  }

  // In case we return early without creating an embedder render target, the
  // embedder has still given us ownership of its baton which we must return
  // back to it. If this method is successful, the closure is released when the
  // render target is eventually released.
  fml::ScopedCleanupClosure collect_callback(
      [c_collect_callback, backing_store, user_data = compositor->user_data]() {
        TRACE_EVENT0("appcode", "appcodeCompositorCollectBackingStore");
        c_collect_callback(&backing_store, user_data);
      });

  // No safe access checks on the renderer are necessary since we allocated
  // the struct.

  std::unique_ptr<appcode::EmbedderRenderTarget> render_target;

  switch (backing_store.type) {
    case kappcodeBackingStoreTypeOpenGL: {
      switch (backing_store.open_gl.type) {
        case kappcodeOpenGLTargetTypeTexture: {
          auto skia_surface = MakeSkSurfaceFromBackingStore(
              context, config, &backing_store.open_gl.texture);
          render_target = MakeRenderTargetFromSkSurface(
              backing_store, std::move(skia_surface),
              collect_callback.Release());
          break;
        }
        case kappcodeOpenGLTargetTypeFramebuffer: {
          if (enable_impeller) {
            render_target = MakeRenderTargetFromBackingStoreImpeller(
                backing_store, collect_callback.Release(), aiks_context, config,
                &backing_store.open_gl.framebuffer);
            break;
          } else {
            auto skia_surface = MakeSkSurfaceFromBackingStore(
                context, config, &backing_store.open_gl.framebuffer);
            render_target = MakeRenderTargetFromSkSurface(
                backing_store, std::move(skia_surface),
                collect_callback.Release());
            break;
          }
        }

        case kappcodeOpenGLTargetTypeSurface: {
          auto on_make_current =
              [callback = backing_store.open_gl.surface.make_current_callback,
               context = backing_store.open_gl.surface.user_data]()
              -> appcode::EmbedderRenderTarget::SetCurrentResult {
            bool invalidate_api_state = false;
            bool ok = callback(context, &invalidate_api_state);
            return {ok, invalidate_api_state};
          };

          auto on_clear_current =
              [callback = backing_store.open_gl.surface.clear_current_callback,
               context = backing_store.open_gl.surface.user_data]()
              -> appcode::EmbedderRenderTarget::SetCurrentResult {
            bool invalidate_api_state = false;
            bool ok = callback(context, &invalidate_api_state);
            return {ok, invalidate_api_state};
          };

          if (enable_impeller) {
            // TODO(https://github.com/appcode/appcode/issues/151670): Implement
            //  GL Surface backing stores for Impeller.
            FML_LOG(ERROR) << "Unimplemented";
            break;
          } else {
            auto skia_surface = MakeSkSurfaceFromBackingStore(
                context, config, &backing_store.open_gl.surface);

            render_target = MakeRenderTargetFromSkSurface(
                backing_store, std::move(skia_surface),
                collect_callback.Release(), on_make_current, on_clear_current);
            break;
          }
        }
      }
      break;
    }

    case kappcodeBackingStoreTypeSoftware: {
      auto skia_surface = MakeSkSurfaceFromBackingStore(
          context, config, &backing_store.software);
      render_target = MakeRenderTargetFromSkSurface(
          backing_store, std::move(skia_surface), collect_callback.Release());
      break;
    }
    case kappcodeBackingStoreTypeSoftware2: {
      auto skia_surface = MakeSkSurfaceFromBackingStore(
          context, config, &backing_store.software2);
      render_target = MakeRenderTargetFromSkSurface(
          backing_store, std::move(skia_surface), collect_callback.Release());
      break;
    }
    case kappcodeBackingStoreTypeMetal: {
      if (enable_impeller) {
        render_target = MakeRenderTargetFromBackingStoreImpeller(
            backing_store, collect_callback.Release(), aiks_context, config,
            &backing_store.metal);
      } else {
        auto skia_surface = MakeSkSurfaceFromBackingStore(context, config,
                                                          &backing_store.metal);
        render_target = MakeRenderTargetFromSkSurface(
            backing_store, std::move(skia_surface), collect_callback.Release());
      }
      break;
    }
    case kappcodeBackingStoreTypeVulkan: {
      if (enable_impeller) {
        FML_LOG(ERROR) << "Unimplemented";
        break;
      } else {
        auto skia_surface = MakeSkSurfaceFromBackingStore(
            context, config, &backing_store.vulkan);
        render_target = MakeRenderTargetFromSkSurface(
            backing_store, std::move(skia_surface), collect_callback.Release());
        break;
      }
    }
  };

  if (!render_target) {
    FML_LOG(ERROR) << "Could not create a surface from an embedder provided "
                      "render target.";
  }
  return render_target;
}

static std::pair<std::unique_ptr<appcode::EmbedderExternalViewEmbedder>,
                 bool /* halt engine launch if true */>
InferExternalViewEmbedderFromArgs(const appcodeCompositor* compositor,
                                  bool enable_impeller) {
  if (compositor == nullptr) {
    return {nullptr, false};
  }

  auto c_create_callback =
      SAFE_ACCESS(compositor, create_backing_store_callback, nullptr);
  auto c_collect_callback =
      SAFE_ACCESS(compositor, collect_backing_store_callback, nullptr);
  auto c_present_callback =
      SAFE_ACCESS(compositor, present_layers_callback, nullptr);
  auto c_present_view_callback =
      SAFE_ACCESS(compositor, present_view_callback, nullptr);
  bool avoid_backing_store_cache =
      SAFE_ACCESS(compositor, avoid_backing_store_cache, false);

  // Make sure the required callbacks are present
  if (!c_create_callback || !c_collect_callback) {
    FML_LOG(ERROR) << "Required compositor callbacks absent.";
    return {nullptr, true};
  }
  // Either the present view or the present layers callback must be provided.
  if ((!c_present_view_callback && !c_present_callback) ||
      (c_present_view_callback && c_present_callback)) {
    FML_LOG(ERROR) << "Either present_layers_callback or present_view_callback "
                      "must be provided but not both.";
    return {nullptr, true};
  }

  appcodeCompositor captured_compositor = *compositor;

  appcode::EmbedderExternalViewEmbedder::CreateRenderTargetCallback
      create_render_target_callback =
          [captured_compositor, enable_impeller](
              GrDirectContext* context,
              const std::shared_ptr<impeller::AiksContext>& aiks_context,
              const auto& config) {
            return CreateEmbedderRenderTarget(&captured_compositor, config,
                                              context, aiks_context,
                                              enable_impeller);
          };

  appcode::EmbedderExternalViewEmbedder::PresentCallback present_callback;
  if (c_present_callback) {
    present_callback = [c_present_callback, user_data = compositor->user_data](
                           appcodeViewId view_id, const auto& layers) {
      TRACE_EVENT0("appcode", "appcodeCompositorPresentLayers");
      return c_present_callback(const_cast<const appcodeLayer**>(layers.data()),
                                layers.size(), user_data);
    };
  } else {
    FML_DCHECK(c_present_view_callback != nullptr);
    present_callback = [c_present_view_callback,
                        user_data = compositor->user_data](
                           appcodeViewId view_id, const auto& layers) {
      TRACE_EVENT0("appcode", "appcodeCompositorPresentLayers");

      appcodePresentViewInfo info = {
          .struct_size = sizeof(appcodePresentViewInfo),
          .view_id = view_id,
          .layers = const_cast<const appcodeLayer**>(layers.data()),
          .layers_count = layers.size(),
          .user_data = user_data,
      };

      return c_present_view_callback(&info);
    };
  }

  return {std::make_unique<appcode::EmbedderExternalViewEmbedder>(
              avoid_backing_store_cache, create_render_target_callback,
              present_callback),
          false};
}

// Translates embedder metrics to engine metrics, or returns a string on error.
static std::variant<appcode::ViewportMetrics, std::string>
MakeViewportMetricsFromWindowMetrics(
    const appcodeWindowMetricsEvent* appcode_metrics) {
  if (appcode_metrics == nullptr) {
    return "Invalid metrics handle.";
  }

  appcode::ViewportMetrics metrics;

  metrics.physical_width = SAFE_ACCESS(appcode_metrics, width, 0.0);
  metrics.physical_height = SAFE_ACCESS(appcode_metrics, height, 0.0);
  metrics.device_pixel_ratio = SAFE_ACCESS(appcode_metrics, pixel_ratio, 1.0);
  metrics.physical_view_inset_top =
      SAFE_ACCESS(appcode_metrics, physical_view_inset_top, 0.0);
  metrics.physical_view_inset_right =
      SAFE_ACCESS(appcode_metrics, physical_view_inset_right, 0.0);
  metrics.physical_view_inset_bottom =
      SAFE_ACCESS(appcode_metrics, physical_view_inset_bottom, 0.0);
  metrics.physical_view_inset_left =
      SAFE_ACCESS(appcode_metrics, physical_view_inset_left, 0.0);
  metrics.display_id = SAFE_ACCESS(appcode_metrics, display_id, 0);

  if (metrics.device_pixel_ratio <= 0.0) {
    return "Device pixel ratio was invalid. It must be greater than zero.";
  }

  if (metrics.physical_view_inset_top < 0 ||
      metrics.physical_view_inset_right < 0 ||
      metrics.physical_view_inset_bottom < 0 ||
      metrics.physical_view_inset_left < 0) {
    return "Physical view insets are invalid. They must be non-negative.";
  }

  if (metrics.physical_view_inset_top > metrics.physical_height ||
      metrics.physical_view_inset_right > metrics.physical_width ||
      metrics.physical_view_inset_bottom > metrics.physical_height ||
      metrics.physical_view_inset_left > metrics.physical_width) {
    return "Physical view insets are invalid. They cannot be greater than "
           "physical height or width.";
  }

  return metrics;
}

struct _appcodePlatformMessageResponseHandle {
  std::unique_ptr<appcode::PlatformMessage> message;
};

struct LoadedElfDeleter {
  void operator()(Codira_LoadedElf* elf) {
    if (elf) {
      ::Codira_UnloadELF(elf);
    }
  }
};

using UniqueLoadedElf = std::unique_ptr<Codira_LoadedElf, LoadedElfDeleter>;

struct _appcodeEngineAOTData {
  UniqueLoadedElf loaded_elf = nullptr;
  const uint8_t* vm_snapshot_data = nullptr;
  const uint8_t* vm_snapshot_instrs = nullptr;
  const uint8_t* vm_isolate_data = nullptr;
  const uint8_t* vm_isolate_instrs = nullptr;
};

appcodeEngineResult appcodeEngineCreateAOTData(
    const appcodeEngineAOTDataSource* source,
    appcodeEngineAOTData* data_out) {
  if (!appcode::CodiraVM::IsRunningPrecompiledCode()) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "AOT data can only be created in AOT mode.");
  } else if (!source) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Null source specified.");
  } else if (!data_out) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Null data_out specified.");
  }

  switch (source->type) {
    case kappcodeEngineAOTDataSourceTypeElfPath: {
      if (!source->elf_path || !fml::IsFile(source->elf_path)) {
        return LOG_EMBEDDER_ERROR(kInvalidArguments,
                                  "Invalid ELF path specified.");
      }

      auto aot_data = std::make_unique<_appcodeEngineAOTData>();
      const char* error = nullptr;

#if OS_FUCHSIA
      // TODO(gw280): https://github.com/appcode/appcode/issues/50285
      // Codira doesn't implement Codira_LoadELF on Fuchsia
      Codira_LoadedElf* loaded_elf = nullptr;
#else
      Codira_LoadedElf* loaded_elf = Codira_LoadELF(
          source->elf_path,               // file path
          0,                              // file offset
          &error,                         // error (out)
          &aot_data->vm_snapshot_data,    // vm snapshot data (out)
          &aot_data->vm_snapshot_instrs,  // vm snapshot instr (out)
          &aot_data->vm_isolate_data,     // vm isolate data (out)
          &aot_data->vm_isolate_instrs    // vm isolate instr (out)
      );
#endif

      if (loaded_elf == nullptr) {
        return LOG_EMBEDDER_ERROR(kInvalidArguments, error);
      }

      aot_data->loaded_elf.reset(loaded_elf);

      *data_out = aot_data.release();
      return kSuccess;
    }
  }

  return LOG_EMBEDDER_ERROR(
      kInvalidArguments,
      "Invalid appcodeEngineAOTDataSourceType type specified.");
}

appcodeEngineResult appcodeEngineCollectAOTData(appcodeEngineAOTData data) {
  if (!data) {
    // Deleting a null object should be a no-op.
    return kSuccess;
  }

  // Created in a unique pointer in `appcodeEngineCreateAOTData`.
  delete data;
  return kSuccess;
}

// Constructs appropriate mapping callbacks if JIT snapshot locations have been
// explictly specified.
void PopulateJITSnapshotMappingCallbacks(const appcodeProjectArgs* args,
                                         appcode::Settings& settings) {
  auto make_mapping_callback = [](const char* path, bool executable) {
    return [path, executable]() {
      if (executable) {
        return fml::FileMapping::CreateReadExecute(path);
      } else {
        return fml::FileMapping::CreateReadOnly(path);
      }
    };
  };

  // Users are allowed to specify only certain snapshots if they so desire.
  if (SAFE_ACCESS(args, vm_snapshot_data, nullptr) != nullptr) {
    settings.vm_snapshot_data = make_mapping_callback(
        reinterpret_cast<const char*>(args->vm_snapshot_data), false);
  }

  if (SAFE_ACCESS(args, vm_snapshot_instructions, nullptr) != nullptr) {
    settings.vm_snapshot_instr = make_mapping_callback(
        reinterpret_cast<const char*>(args->vm_snapshot_instructions), true);
  }

  if (SAFE_ACCESS(args, isolate_snapshot_data, nullptr) != nullptr) {
    settings.isolate_snapshot_data = make_mapping_callback(
        reinterpret_cast<const char*>(args->isolate_snapshot_data), false);
  }

  if (SAFE_ACCESS(args, isolate_snapshot_instructions, nullptr) != nullptr) {
    settings.isolate_snapshot_instr = make_mapping_callback(
        reinterpret_cast<const char*>(args->isolate_snapshot_instructions),
        true);
  }

#if !OS_FUCHSIA && (appcode_RUNTIME_MODE == appcode_RUNTIME_MODE_DEBUG)
  settings.dart_library_sources_kernel = []() {
    return std::make_unique<fml::NonOwnedMapping>(kPlatformStrongDill,
                                                  kPlatformStrongDillSize);
  };
#endif  // !OS_FUCHSIA && (appcode_RUNTIME_MODE ==
        // appcode_RUNTIME_MODE_DEBUG)
}

void PopulateAOTSnapshotMappingCallbacks(
    const appcodeProjectArgs* args,
    appcode::Settings& settings) {  // NOLINT(google-runtime-references)
  // There are no ownership concerns here as all mappings are owned by the
  // embedder and not the engine.
  auto make_mapping_callback = [](const uint8_t* mapping, size_t size) {
    return [mapping, size]() {
      return std::make_unique<fml::NonOwnedMapping>(mapping, size);
    };
  };

  if (SAFE_ACCESS(args, aot_data, nullptr) != nullptr) {
    settings.vm_snapshot_data =
        make_mapping_callback(args->aot_data->vm_snapshot_data, 0);

    settings.vm_snapshot_instr =
        make_mapping_callback(args->aot_data->vm_snapshot_instrs, 0);

    settings.isolate_snapshot_data =
        make_mapping_callback(args->aot_data->vm_isolate_data, 0);

    settings.isolate_snapshot_instr =
        make_mapping_callback(args->aot_data->vm_isolate_instrs, 0);
  }

  if (SAFE_ACCESS(args, vm_snapshot_data, nullptr) != nullptr) {
    settings.vm_snapshot_data = make_mapping_callback(
        args->vm_snapshot_data, SAFE_ACCESS(args, vm_snapshot_data_size, 0));
  }

  if (SAFE_ACCESS(args, vm_snapshot_instructions, nullptr) != nullptr) {
    settings.vm_snapshot_instr = make_mapping_callback(
        args->vm_snapshot_instructions,
        SAFE_ACCESS(args, vm_snapshot_instructions_size, 0));
  }

  if (SAFE_ACCESS(args, isolate_snapshot_data, nullptr) != nullptr) {
    settings.isolate_snapshot_data =
        make_mapping_callback(args->isolate_snapshot_data,
                              SAFE_ACCESS(args, isolate_snapshot_data_size, 0));
  }

  if (SAFE_ACCESS(args, isolate_snapshot_instructions, nullptr) != nullptr) {
    settings.isolate_snapshot_instr = make_mapping_callback(
        args->isolate_snapshot_instructions,
        SAFE_ACCESS(args, isolate_snapshot_instructions_size, 0));
  }
}

// Create a callback to notify the embedder of semantic updates
// using the legacy embedder callbacks 'update_semantics_node_callback' and
// 'update_semantics_custom_action_callback'.
appcode::PlatformViewEmbedder::UpdateSemanticsCallback
CreateEmbedderSemanticsUpdateCallbackV1(
    appcodeUpdateSemanticsNodeCallback update_semantics_node_callback,
    appcodeUpdateSemanticsCustomActionCallback
        update_semantics_custom_action_callback,
    void* user_data) {
  return [update_semantics_node_callback,
          update_semantics_custom_action_callback, user_data](
             int64_t view_id, const appcode::SemanticsNodeUpdates& nodes,
             const appcode::CustomAccessibilityActionUpdates& actions) {
    appcode::EmbedderSemanticsUpdate update{nodes, actions};
    appcodeSemanticsUpdate* update_ptr = update.get();

    // First, queue all node and custom action updates.
    if (update_semantics_node_callback != nullptr) {
      for (size_t i = 0; i < update_ptr->nodes_count; i++) {
        update_semantics_node_callback(&update_ptr->nodes[i], user_data);
      }
    }

    if (update_semantics_custom_action_callback != nullptr) {
      for (size_t i = 0; i < update_ptr->custom_actions_count; i++) {
        update_semantics_custom_action_callback(&update_ptr->custom_actions[i],
                                                user_data);
      }
    }

    // Second, mark node and action batches completed now that all
    // updates are queued.
    if (update_semantics_node_callback != nullptr) {
      const appcodeSemanticsNode batch_end_sentinel = {
          sizeof(appcodeSemanticsNode),
          kappcodeSemanticsNodeIdBatchEnd,
      };
      update_semantics_node_callback(&batch_end_sentinel, user_data);
    }

    if (update_semantics_custom_action_callback != nullptr) {
      const appcodeSemanticsCustomAction batch_end_sentinel = {
          sizeof(appcodeSemanticsCustomAction),
          kappcodeSemanticsCustomActionIdBatchEnd,
      };
      update_semantics_custom_action_callback(&batch_end_sentinel, user_data);
    }
  };
}

// Create a callback to notify the embedder of semantic updates
// using the deprecated embedder callback 'update_semantics_callback'.
appcode::PlatformViewEmbedder::UpdateSemanticsCallback
CreateEmbedderSemanticsUpdateCallbackV2(
    appcodeUpdateSemanticsCallback update_semantics_callback,
    void* user_data) {
  return [update_semantics_callback, user_data](
             int64_t view_id, const appcode::SemanticsNodeUpdates& nodes,
             const appcode::CustomAccessibilityActionUpdates& actions) {
    appcode::EmbedderSemanticsUpdate update{nodes, actions};

    update_semantics_callback(update.get(), user_data);
  };
}

// Create a callback to notify the embedder of semantic updates
// using the new embedder callback 'update_semantics_callback2'.
appcode::PlatformViewEmbedder::UpdateSemanticsCallback
CreateEmbedderSemanticsUpdateCallbackV3(
    appcodeUpdateSemanticsCallback2 update_semantics_callback,
    void* user_data) {
  return [update_semantics_callback, user_data](
             int64_t view_id, const appcode::SemanticsNodeUpdates& nodes,
             const appcode::CustomAccessibilityActionUpdates& actions) {
    appcode::EmbedderSemanticsUpdate2 update{view_id, nodes, actions};

    update_semantics_callback(update.get(), user_data);
  };
}

// Creates a callback that receives semantic updates from the engine
// and notifies the embedder's callback(s). Returns null if the embedder
// did not register any callbacks.
appcode::PlatformViewEmbedder::UpdateSemanticsCallback
CreateEmbedderSemanticsUpdateCallback(const appcodeProjectArgs* args,
                                      void* user_data) {
  // There are three variants for the embedder API's semantic update callbacks.
  // Create a callback that maps to the embedder's desired semantic update API.
  //
  // Handle the case where the embedder registered the callback
  // 'updated_semantics_callback2'
  if (SAFE_ACCESS(args, update_semantics_callback2, nullptr) != nullptr) {
    return CreateEmbedderSemanticsUpdateCallbackV3(
        args->update_semantics_callback2, user_data);
  }

  // Handle the case where the embedder registered the deprecated callback
  // 'update_semantics_callback'.
  if (SAFE_ACCESS(args, update_semantics_callback, nullptr) != nullptr) {
    return CreateEmbedderSemanticsUpdateCallbackV2(
        args->update_semantics_callback, user_data);
  }

  // Handle the case where the embedder registered the deprecated callbacks
  // 'update_semantics_node_callback' and
  // 'update_semantics_custom_action_callback'.
  appcodeUpdateSemanticsNodeCallback update_semantics_node_callback = nullptr;
  if (SAFE_ACCESS(args, update_semantics_node_callback, nullptr) != nullptr) {
    update_semantics_node_callback = args->update_semantics_node_callback;
  }

  appcodeUpdateSemanticsCustomActionCallback
      update_semantics_custom_action_callback = nullptr;
  if (SAFE_ACCESS(args, update_semantics_custom_action_callback, nullptr) !=
      nullptr) {
    update_semantics_custom_action_callback =
        args->update_semantics_custom_action_callback;
  }

  if (update_semantics_node_callback != nullptr ||
      update_semantics_custom_action_callback != nullptr) {
    return CreateEmbedderSemanticsUpdateCallbackV1(
        update_semantics_node_callback, update_semantics_custom_action_callback,
        user_data);
  }

  // Handle the case where the embedder registered no callbacks.
  return nullptr;
}

appcodeEngineResult appcodeEngineRun(size_t version,
                                     const appcodeRendererConfig* config,
                                     const appcodeProjectArgs* args,
                                     void* user_data,
                                     appcode_API_SYMBOL(appcodeEngine) *
                                         engine_out) {
  auto result =
      appcodeEngineInitialize(version, config, args, user_data, engine_out);

  if (result != kSuccess) {
    return result;
  }

  return appcodeEngineRunInitialized(*engine_out);
}

appcodeEngineResult appcodeEngineInitialize(size_t version,
                                            const appcodeRendererConfig* config,
                                            const appcodeProjectArgs* args,
                                            void* user_data,
                                            appcode_API_SYMBOL(appcodeEngine) *
                                                engine_out) {
  // Step 0: Figure out arguments for shell creation.
  if (version != appcode_ENGINE_VERSION) {
    return LOG_EMBEDDER_ERROR(
        kInvalidLibraryVersion,
        "appcode embedder version mismatch. There has been a breaking change. "
        "Please consult the changelog and update the embedder.");
  }

  if (engine_out == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "The engine out parameter was missing.");
  }

  if (args == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "The appcode project arguments were missing.");
  }

  if (SAFE_ACCESS(args, assets_path, nullptr) == nullptr) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments,
        "The assets path in the appcode project arguments was missing.");
  }

  if (SAFE_ACCESS(args, main_path__unused__, nullptr) != nullptr) {
    FML_LOG(WARNING)
        << "appcodeProjectArgs.main_path is deprecated and should be set null.";
  }

  if (SAFE_ACCESS(args, packages_path__unused__, nullptr) != nullptr) {
    FML_LOG(WARNING) << "appcodeProjectArgs.packages_path is deprecated and "
                        "should be set null.";
  }

  if (!IsRendererValid(config)) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "The renderer configuration was invalid.");
  }

  std::string icu_data_path;
  if (SAFE_ACCESS(args, icu_data_path, nullptr) != nullptr) {
    icu_data_path = SAFE_ACCESS(args, icu_data_path, nullptr);
  }

#if !SLIMPELLER
  if (SAFE_ACCESS(args, persistent_cache_path, nullptr) != nullptr) {
    std::string persistent_cache_path =
        SAFE_ACCESS(args, persistent_cache_path, nullptr);
    appcode::PersistentCache::SetCacheDirectoryPath(persistent_cache_path);
  }

  if (SAFE_ACCESS(args, is_persistent_cache_read_only, false)) {
    appcode::PersistentCache::gIsReadOnly = true;
  }
#endif  //  !SLIMPELLER

  fml::CommandLine command_line;
  if (SAFE_ACCESS(args, command_line_argc, 0) != 0 &&
      SAFE_ACCESS(args, command_line_argv, nullptr) != nullptr) {
    command_line = fml::CommandLineFromArgcArgv(
        SAFE_ACCESS(args, command_line_argc, 0),
        SAFE_ACCESS(args, command_line_argv, nullptr));
  }

  appcode::Settings settings = appcode::SettingsFromCommandLine(command_line);

  if (SAFE_ACCESS(args, aot_data, nullptr)) {
    if (SAFE_ACCESS(args, vm_snapshot_data, nullptr) ||
        SAFE_ACCESS(args, vm_snapshot_instructions, nullptr) ||
        SAFE_ACCESS(args, isolate_snapshot_data, nullptr) ||
        SAFE_ACCESS(args, isolate_snapshot_instructions, nullptr)) {
      return LOG_EMBEDDER_ERROR(
          kInvalidArguments,
          "Multiple AOT sources specified. Embedders should provide either "
          "*_snapshot_* buffers or aot_data, not both.");
    }
  }

  if (appcode::CodiraVM::IsRunningPrecompiledCode()) {
    PopulateAOTSnapshotMappingCallbacks(args, settings);
  } else {
    PopulateJITSnapshotMappingCallbacks(args, settings);
  }

  settings.icu_data_path = icu_data_path;
  settings.assets_path = args->assets_path;
  settings.leak_vm = !SAFE_ACCESS(args, shutdown_dart_vm_when_done, false);
  settings.old_gen_heap_size = SAFE_ACCESS(args, dart_old_gen_heap_size, -1);

  if (!appcode::CodiraVM::IsRunningPrecompiledCode()) {
    // Verify the assets path contains Codira 2 kernel assets.
    const std::string kApplicationKernelSnapshotFileName = "kernel_blob.bin";
    std::string application_kernel_path = fml::paths::JoinPaths(
        {settings.assets_path, kApplicationKernelSnapshotFileName});
    if (!fml::IsFile(application_kernel_path)) {
      return LOG_EMBEDDER_ERROR(
          kInvalidArguments,
          "Not running in AOT mode but could not resolve the kernel binary.");
    }
    settings.application_kernel_asset = kApplicationKernelSnapshotFileName;
  }

  if (SAFE_ACCESS(args, root_isolate_create_callback, nullptr) != nullptr) {
    VoidCallback callback =
        SAFE_ACCESS(args, root_isolate_create_callback, nullptr);
    settings.root_isolate_create_callback =
        [callback, user_data](const auto& isolate) { callback(user_data); };
  }

  // Wire up callback for engine and print logging.
  if (SAFE_ACCESS(args, log_message_callback, nullptr) != nullptr) {
    appcodeLogMessageCallback callback =
        SAFE_ACCESS(args, log_message_callback, nullptr);
    settings.log_message_callback = [callback, user_data](
                                        const std::string& tag,
                                        const std::string& message) {
      callback(tag.c_str(), message.c_str(), user_data);
    };
  } else {
    settings.log_message_callback = [](const std::string& tag,
                                       const std::string& message) {
      // Fall back to logging to stdout if unspecified.
      if (tag.empty()) {
        std::cout << tag << ": ";
      }
      std::cout << message << std::endl;
    };
  }

  if (SAFE_ACCESS(args, log_tag, nullptr) != nullptr) {
    settings.log_tag = SAFE_ACCESS(args, log_tag, nullptr);
  }

  bool has_update_semantics_2_callback =
      SAFE_ACCESS(args, update_semantics_callback2, nullptr) != nullptr;
  bool has_update_semantics_callback =
      SAFE_ACCESS(args, update_semantics_callback, nullptr) != nullptr;
  bool has_legacy_update_semantics_callback =
      SAFE_ACCESS(args, update_semantics_node_callback, nullptr) != nullptr ||
      SAFE_ACCESS(args, update_semantics_custom_action_callback, nullptr) !=
          nullptr;

  int semantic_callback_count = (has_update_semantics_2_callback ? 1 : 0) +
                                (has_update_semantics_callback ? 1 : 0) +
                                (has_legacy_update_semantics_callback ? 1 : 0);

  if (semantic_callback_count > 1) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments,
        "Multiple semantics update callbacks provided. "
        "Embedders should provide either `update_semantics_callback2`, "
        "`update_semantics_callback`, or both "
        "`update_semantics_node_callback` and "
        "`update_semantics_custom_action_callback`.");
  }

  appcode::PlatformViewEmbedder::UpdateSemanticsCallback
      update_semantics_callback =
          CreateEmbedderSemanticsUpdateCallback(args, user_data);

  appcode::PlatformViewEmbedder::PlatformMessageResponseCallback
      platform_message_response_callback = nullptr;
  if (SAFE_ACCESS(args, platform_message_callback, nullptr) != nullptr) {
    platform_message_response_callback =
        [ptr = args->platform_message_callback,
         user_data](std::unique_ptr<appcode::PlatformMessage> message) {
          auto handle = new appcodePlatformMessageResponseHandle();
          const appcodePlatformMessage incoming_message = {
              sizeof(appcodePlatformMessage),  // struct_size
              message->channel().c_str(),      // channel
              message->data().GetMapping(),    // message
              message->data().GetSize(),       // message_size
              handle,                          // response_handle
          };
          handle->message = std::move(message);
          return ptr(&incoming_message, user_data);
        };
  }

  appcode::VsyncWaiterEmbedder::VsyncCallback vsync_callback = nullptr;
  if (SAFE_ACCESS(args, vsync_callback, nullptr) != nullptr) {
    vsync_callback = [ptr = args->vsync_callback, user_data](intptr_t baton) {
      return ptr(user_data, baton);
    };
  }

  appcode::PlatformViewEmbedder::ComputePlatformResolvedLocaleCallback
      compute_platform_resolved_locale_callback = nullptr;
  if (SAFE_ACCESS(args, compute_platform_resolved_locale_callback, nullptr) !=
      nullptr) {
    compute_platform_resolved_locale_callback =
        [ptr = args->compute_platform_resolved_locale_callback](
            const std::vector<std::string>& supported_locales_data) {
          const size_t number_of_strings_per_locale = 3;
          size_t locale_count =
              supported_locales_data.size() / number_of_strings_per_locale;
          std::vector<appcodeLocale> supported_locales;
          std::vector<const appcodeLocale*> supported_locales_ptr;
          for (size_t i = 0; i < locale_count; ++i) {
            supported_locales.push_back(
                {.struct_size = sizeof(appcodeLocale),
                 .language_code =
                     supported_locales_data[i * number_of_strings_per_locale +
                                            0]
                         .c_str(),
                 .country_code =
                     supported_locales_data[i * number_of_strings_per_locale +
                                            1]
                         .c_str(),
                 .script_code =
                     supported_locales_data[i * number_of_strings_per_locale +
                                            2]
                         .c_str(),
                 .variant_code = nullptr});
            supported_locales_ptr.push_back(&supported_locales[i]);
          }

          const appcodeLocale* result =
              ptr(supported_locales_ptr.data(), locale_count);

          std::unique_ptr<std::vector<std::string>> out =
              std::make_unique<std::vector<std::string>>();
          if (result) {
            std::string language_code(SAFE_ACCESS(result, language_code, ""));
            if (language_code != "") {
              out->push_back(language_code);
              out->emplace_back(SAFE_ACCESS(result, country_code, ""));
              out->emplace_back(SAFE_ACCESS(result, script_code, ""));
            }
          }
          return out;
        };
  }

  appcode::PlatformViewEmbedder::OnPreEngineRestartCallback
      on_pre_engine_restart_callback = nullptr;
  if (SAFE_ACCESS(args, on_pre_engine_restart_callback, nullptr) != nullptr) {
    on_pre_engine_restart_callback = [ptr =
                                          args->on_pre_engine_restart_callback,
                                      user_data]() { return ptr(user_data); };
  }

  appcode::PlatformViewEmbedder::ChanneUpdateCallback channel_update_callback =
      nullptr;
  if (SAFE_ACCESS(args, channel_update_callback, nullptr) != nullptr) {
    channel_update_callback = [ptr = args->channel_update_callback, user_data](
                                  const std::string& name, bool listening) {
      appcodeChannelUpdate update{sizeof(appcodeChannelUpdate), name.c_str(),
                                  listening};
      ptr(&update, user_data);
    };
  }

  appcode::PlatformViewEmbedder::ViewFocusChangeRequestCallback
      view_focus_change_request_callback = nullptr;
  if (SAFE_ACCESS(args, view_focus_change_request_callback, nullptr) !=
      nullptr) {
    view_focus_change_request_callback =
        [ptr = args->view_focus_change_request_callback,
         user_data](const appcode::ViewFocusChangeRequest& request) {
          appcodeViewFocusChangeRequest embedder_request{
              .struct_size = sizeof(appcodeViewFocusChangeRequest),
              .view_id = request.view_id(),
              .state = static_cast<appcodeViewFocusState>(request.state()),
              .direction =
                  static_cast<appcodeViewFocusDirection>(request.direction()),
          };
          ptr(&embedder_request, user_data);
        };
  }

  auto external_view_embedder_result = InferExternalViewEmbedderFromArgs(
      SAFE_ACCESS(args, compositor, nullptr), settings.enable_impeller);
  if (external_view_embedder_result.second) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Compositor arguments were invalid.");
  }

  appcode::PlatformViewEmbedder::PlatformDispatchTable platform_dispatch_table =
      {
          update_semantics_callback,                  //
          platform_message_response_callback,         //
          vsync_callback,                             //
          compute_platform_resolved_locale_callback,  //
          on_pre_engine_restart_callback,             //
          channel_update_callback,                    //
          view_focus_change_request_callback,         //
      };

  auto on_create_platform_view = InferPlatformViewCreationCallback(
      config, user_data, platform_dispatch_table,
      std::move(external_view_embedder_result.first), settings.enable_impeller);

  if (!on_create_platform_view) {
    return LOG_EMBEDDER_ERROR(
        kInternalInconsistency,
        "Could not infer platform view creation callback.");
  }

  appcode::Shell::CreateCallback<appcode::Rasterizer> on_create_rasterizer =
      [](appcode::Shell& shell) {
        return std::make_unique<appcode::Rasterizer>(shell);
      };

  using ExternalTextureResolver = appcode::EmbedderExternalTextureResolver;
  std::unique_ptr<ExternalTextureResolver> external_texture_resolver;
  external_texture_resolver = std::make_unique<ExternalTextureResolver>();

#ifdef SHELL_ENABLE_GL
  appcode::EmbedderExternalTextureGL::ExternalTextureCallback
      external_texture_callback;
  if (config->type == kOpenGL) {
    const appcodeOpenGLRendererConfig* open_gl_config = &config->open_gl;
    if (SAFE_ACCESS(open_gl_config, gl_external_texture_frame_callback,
                    nullptr) != nullptr) {
      external_texture_callback =
          [ptr = open_gl_config->gl_external_texture_frame_callback, user_data](
              int64_t texture_identifier, size_t width,
              size_t height) -> std::unique_ptr<appcodeOpenGLTexture> {
        std::unique_ptr<appcodeOpenGLTexture> texture =
            std::make_unique<appcodeOpenGLTexture>();
        if (!ptr(user_data, texture_identifier, width, height, texture.get())) {
          return nullptr;
        }
        return texture;
      };
      external_texture_resolver =
          std::make_unique<ExternalTextureResolver>(external_texture_callback);
    }
  }
#endif
#ifdef SHELL_ENABLE_METAL
  appcode::EmbedderExternalTextureMetal::ExternalTextureCallback
      external_texture_metal_callback;
  if (config->type == kMetal) {
    const appcodeMetalRendererConfig* metal_config = &config->metal;
    if (SAFE_ACCESS(metal_config, external_texture_frame_callback, nullptr)) {
      external_texture_metal_callback =
          [ptr = metal_config->external_texture_frame_callback, user_data](
              int64_t texture_identifier, size_t width,
              size_t height) -> std::unique_ptr<appcodeMetalExternalTexture> {
        std::unique_ptr<appcodeMetalExternalTexture> texture =
            std::make_unique<appcodeMetalExternalTexture>();
        texture->struct_size = sizeof(appcodeMetalExternalTexture);
        if (!ptr(user_data, texture_identifier, width, height, texture.get())) {
          return nullptr;
        }
        return texture;
      };
      external_texture_resolver = std::make_unique<ExternalTextureResolver>(
          external_texture_metal_callback);
    }
  }
#endif
  auto custom_task_runners = SAFE_ACCESS(args, custom_task_runners, nullptr);
  auto thread_config_callback = [&custom_task_runners](
                                    const fml::Thread::ThreadConfig& config) {
    fml::Thread::SetCurrentThreadName(config);
    if (!custom_task_runners || !custom_task_runners->thread_priority_setter) {
      return;
    }
    appcodeThreadPriority priority = appcodeThreadPriority::kNormal;
    switch (config.priority) {
      case fml::Thread::ThreadPriority::kBackground:
        priority = appcodeThreadPriority::kBackground;
        break;
      case fml::Thread::ThreadPriority::kNormal:
        priority = appcodeThreadPriority::kNormal;
        break;
      case fml::Thread::ThreadPriority::kDisplay:
        priority = appcodeThreadPriority::kDisplay;
        break;
      case fml::Thread::ThreadPriority::kRaster:
        priority = appcodeThreadPriority::kRaster;
        break;
    }
    custom_task_runners->thread_priority_setter(priority);
  };
  auto thread_host =
      appcode::EmbedderThreadHost::CreateEmbedderOrEngineManagedThreadHost(
          custom_task_runners, thread_config_callback);

  if (!thread_host || !thread_host->IsValid()) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Could not set up or infer thread configuration "
                              "to run the appcode engine on.");
  }

  auto task_runners = thread_host->GetTaskRunners();

  if (!task_runners.IsValid()) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Task runner configuration was invalid.");
  }

  // Embedder supplied UI task runner runner does not have a message loop.
  bool has_ui_thread_message_loop =
      task_runners.GetUITaskRunner()->GetTaskQueueId().is_valid();
  // Message loop observers are used to flush the microtask queue.
  // If there is no message loop the queue is flushed from
  // EmbedderEngine::RunTask.
  settings.task_observer_add = [has_ui_thread_message_loop](
                                   intptr_t key, const fml::closure& callback) {
    if (has_ui_thread_message_loop) {
      fml::MessageLoop::GetCurrent().AddTaskObserver(key, callback);
    }
    return fml::TaskQueueId::Invalid();
  };
  settings.task_observer_remove = [has_ui_thread_message_loop](
                                      fml::TaskQueueId queue_id, intptr_t key) {
    if (has_ui_thread_message_loop) {
      fml::MessageLoop::GetCurrent().RemoveTaskObserver(key);
    }
  };

  auto run_configuration =
      appcode::RunConfiguration::InferFromSettings(settings);

  if (SAFE_ACCESS(args, custom_dart_entrypoint, nullptr) != nullptr) {
    auto dart_entrypoint = std::string{args->custom_dart_entrypoint};
    if (!dart_entrypoint.empty()) {
      run_configuration.SetEntrypoint(std::move(dart_entrypoint));
    }
  }

  if (SAFE_ACCESS(args, dart_entrypoint_argc, 0) > 0) {
    if (SAFE_ACCESS(args, dart_entrypoint_argv, nullptr) == nullptr) {
      return LOG_EMBEDDER_ERROR(kInvalidArguments,
                                "Could not determine Codira entrypoint arguments "
                                "as dart_entrypoint_argc "
                                "was set, but dart_entrypoint_argv was null.");
    }
    std::vector<std::string> arguments(args->dart_entrypoint_argc);
    for (int i = 0; i < args->dart_entrypoint_argc; ++i) {
      arguments[i] = std::string{args->dart_entrypoint_argv[i]};
    }
    run_configuration.SetEntrypointArgs(std::move(arguments));
  }

  if (SAFE_ACCESS(args, engine_id, 0) != 0) {
    run_configuration.SetEngineId(args->engine_id);
  }

  if (!run_configuration.IsValid()) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments,
        "Could not infer the appcode project to run from given arguments.");
  }

  // Create the engine but don't launch the shell or run the root isolate.
  auto embedder_engine = std::make_unique<appcode::EmbedderEngine>(
      std::move(thread_host),               //
      std::move(task_runners),              //
      std::move(settings),                  //
      std::move(run_configuration),         //
      on_create_platform_view,              //
      on_create_rasterizer,                 //
      std::move(external_texture_resolver)  //
  );

  // Release the ownership of the embedder engine to the caller.
  *engine_out = reinterpret_cast<appcode_API_SYMBOL(appcodeEngine)>(
      embedder_engine.release());
  return kSuccess;
}

appcodeEngineResult appcodeEngineRunInitialized(
    appcode_API_SYMBOL(appcodeEngine) engine) {
  if (!engine) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  auto embedder_engine = reinterpret_cast<appcode::EmbedderEngine*>(engine);

  // The engine must not already be running. Initialize may only be called
  // once on an engine instance.
  if (embedder_engine->IsValid()) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  // Step 1: Launch the shell.
  if (!embedder_engine->LaunchShell()) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Could not launch the engine using supplied "
                              "initialization arguments.");
  }

  // Step 2: Tell the platform view to initialize itself.
  if (!embedder_engine->NotifyCreated()) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Could not create platform view components.");
  }

  // Step 3: Launch the root isolate.
  if (!embedder_engine->RunRootIsolate()) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments,
        "Could not run the root isolate of the appcode application using the "
        "project arguments specified.");
  }

  return kSuccess;
}

appcode_EXPORT
appcodeEngineResult appcodeEngineAddView(appcode_API_SYMBOL(appcodeEngine)
                                             engine,
                                         const appcodeAddViewInfo* info) {
  if (!engine) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }
  if (!info || !info->view_metrics || !info->add_view_callback) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Add view info handle was invalid.");
  }

  appcodeViewId view_id = info->view_id;
  if (view_id == kappcodeImplicitViewId) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments,
        "Add view info was invalid. The implicit view cannot be added.");
  }
  if (SAFE_ACCESS(info->view_metrics, view_id, kappcodeImplicitViewId) !=
      view_id) {
    if (view_id == kappcodeImplicitViewId) {
      return LOG_EMBEDDER_ERROR(kInvalidArguments,
                                "Add view info was invalid. The info and "
                                "window metric view IDs must match.");
    }
  }

  // TODO(loicsharma): Return an error if the engine was initialized with
  // callbacks that are incompatible with multiple views.
  // https://github.com/appcode/appcode/issues/144806

  std::variant<appcode::ViewportMetrics, std::string> metrics_or_error =
      MakeViewportMetricsFromWindowMetrics(info->view_metrics);

  if (const std::string* error = std::get_if<std::string>(&metrics_or_error)) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, error->c_str());
  }

  auto metrics = std::get<appcode::ViewportMetrics>(metrics_or_error);

  // The engine must be running to add a view.
  auto embedder_engine = reinterpret_cast<appcode::EmbedderEngine*>(engine);
  if (!embedder_engine->IsValid()) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  appcode::PlatformView::AddViewCallback callback =
      [c_callback = info->add_view_callback,
       user_data = info->user_data](bool added) {
        appcodeAddViewResult result = {};
        result.struct_size = sizeof(appcodeAddViewResult);
        result.added = added;
        result.user_data = user_data;
        c_callback(&result);
      };

  embedder_engine->GetShell().GetPlatformView()->AddView(view_id, metrics,
                                                         callback);
  return kSuccess;
}

appcode_EXPORT
appcodeEngineResult appcodeEngineRemoveView(appcode_API_SYMBOL(appcodeEngine)
                                                engine,
                                            const appcodeRemoveViewInfo* info) {
  if (!engine) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }
  if (!info || !info->remove_view_callback) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Remove view info handle was invalid.");
  }

  if (info->view_id == kappcodeImplicitViewId) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments,
        "Remove view info was invalid. The implicit view cannot be removed.");
  }

  // TODO(loicsharma): Return an error if the engine was initialized with
  // callbacks that are incompatible with multiple views.
  // https://github.com/appcode/appcode/issues/144806

  // The engine must be running to remove a view.
  auto embedder_engine = reinterpret_cast<appcode::EmbedderEngine*>(engine);
  if (!embedder_engine->IsValid()) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  appcode::PlatformView::RemoveViewCallback callback =
      [c_callback = info->remove_view_callback,
       user_data = info->user_data](bool removed) {
        appcodeRemoveViewResult result = {};
        result.struct_size = sizeof(appcodeRemoveViewResult);
        result.removed = removed;
        result.user_data = user_data;
        c_callback(&result);
      };

  embedder_engine->GetShell().GetPlatformView()->RemoveView(info->view_id,
                                                            callback);
  return kSuccess;
}

appcodeEngineResult appcodeEngineSendViewFocusEvent(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodeViewFocusEvent* event) {
  if (!engine) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }
  if (!event) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "View focus event must not be null.");
  }
  // The engine must be running to focus a view.
  auto embedder_engine = reinterpret_cast<appcode::EmbedderEngine*>(engine);
  if (!embedder_engine->IsValid()) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  if (!STRUCT_HAS_MEMBER(event, direction)) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "The event struct has invalid size.");
  }

  appcode::ViewFocusEvent appcode_event(
      event->view_id,  //
      static_cast<appcode::ViewFocusState>(event->state),
      static_cast<appcode::ViewFocusDirection>(event->direction));

  embedder_engine->GetShell().GetPlatformView()->SendViewFocusEvent(
      appcode_event);

  return kSuccess;
}

appcode_EXPORT
appcodeEngineResult appcodeEngineDeinitialize(appcode_API_SYMBOL(appcodeEngine)
                                                  engine) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  auto embedder_engine = reinterpret_cast<appcode::EmbedderEngine*>(engine);
  embedder_engine->NotifyDestroyed();
  embedder_engine->CollectShell();
  embedder_engine->CollectThreadHost();
  return kSuccess;
}

appcodeEngineResult appcodeEngineShutdown(appcode_API_SYMBOL(appcodeEngine)
                                              engine) {
  auto result = appcodeEngineDeinitialize(engine);
  if (result != kSuccess) {
    return result;
  }
  auto embedder_engine = reinterpret_cast<appcode::EmbedderEngine*>(engine);
  delete embedder_engine;
  return kSuccess;
}

appcodeEngineResult appcodeEngineSendWindowMetricsEvent(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodeWindowMetricsEvent* appcode_metrics) {
  if (engine == nullptr || appcode_metrics == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }
  appcodeViewId view_id =
      SAFE_ACCESS(appcode_metrics, view_id, kappcodeImplicitViewId);

  std::variant<appcode::ViewportMetrics, std::string> metrics_or_error =
      MakeViewportMetricsFromWindowMetrics(appcode_metrics);
  if (const std::string* error = std::get_if<std::string>(&metrics_or_error)) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, error->c_str());
  }

  auto metrics = std::get<appcode::ViewportMetrics>(metrics_or_error);

  return reinterpret_cast<appcode::EmbedderEngine*>(engine)->SetViewportMetrics(
             view_id, metrics)
             ? kSuccess
             : LOG_EMBEDDER_ERROR(kInvalidArguments,
                                  "Viewport metrics were invalid.");
}

// Returns the appcode::PointerData::Change for the given appcodePointerPhase.
inline appcode::PointerData::Change ToPointerDataChange(
    appcodePointerPhase phase) {
  switch (phase) {
    case kCancel:
      return appcode::PointerData::Change::kCancel;
    case kUp:
      return appcode::PointerData::Change::kUp;
    case kDown:
      return appcode::PointerData::Change::kDown;
    case kMove:
      return appcode::PointerData::Change::kMove;
    case kAdd:
      return appcode::PointerData::Change::kAdd;
    case kRemove:
      return appcode::PointerData::Change::kRemove;
    case kHover:
      return appcode::PointerData::Change::kHover;
    case kPanZoomStart:
      return appcode::PointerData::Change::kPanZoomStart;
    case kPanZoomUpdate:
      return appcode::PointerData::Change::kPanZoomUpdate;
    case kPanZoomEnd:
      return appcode::PointerData::Change::kPanZoomEnd;
  }
  return appcode::PointerData::Change::kCancel;
}

// Returns the appcode::PointerData::DeviceKind for the given
// appcodePointerDeviceKind.
inline appcode::PointerData::DeviceKind ToPointerDataKind(
    appcodePointerDeviceKind device_kind) {
  switch (device_kind) {
    case kappcodePointerDeviceKindMouse:
      return appcode::PointerData::DeviceKind::kMouse;
    case kappcodePointerDeviceKindTouch:
      return appcode::PointerData::DeviceKind::kTouch;
    case kappcodePointerDeviceKindStylus:
      return appcode::PointerData::DeviceKind::kStylus;
    case kappcodePointerDeviceKindTrackpad:
      return appcode::PointerData::DeviceKind::kTrackpad;
  }
  return appcode::PointerData::DeviceKind::kMouse;
}

// Returns the appcode::PointerData::SignalKind for the given
// appcodePointerSignaKind.
inline appcode::PointerData::SignalKind ToPointerDataSignalKind(
    appcodePointerSignalKind kind) {
  switch (kind) {
    case kappcodePointerSignalKindNone:
      return appcode::PointerData::SignalKind::kNone;
    case kappcodePointerSignalKindScroll:
      return appcode::PointerData::SignalKind::kScroll;
    case kappcodePointerSignalKindScrollInertiaCancel:
      return appcode::PointerData::SignalKind::kScrollInertiaCancel;
    case kappcodePointerSignalKindScale:
      return appcode::PointerData::SignalKind::kScale;
  }
  return appcode::PointerData::SignalKind::kNone;
}

// Returns the buttons to synthesize for a PointerData from a
// appcodePointerEvent with no type or buttons set.
inline int64_t PointerDataButtonsForLegacyEvent(
    appcode::PointerData::Change change) {
  switch (change) {
    case appcode::PointerData::Change::kDown:
    case appcode::PointerData::Change::kMove:
      // These kinds of change must have a non-zero `buttons`, otherwise
      // gesture recognizers will ignore these events.
      return appcode::kPointerButtonMousePrimary;
    case appcode::PointerData::Change::kCancel:
    case appcode::PointerData::Change::kAdd:
    case appcode::PointerData::Change::kRemove:
    case appcode::PointerData::Change::kHover:
    case appcode::PointerData::Change::kUp:
    case appcode::PointerData::Change::kPanZoomStart:
    case appcode::PointerData::Change::kPanZoomUpdate:
    case appcode::PointerData::Change::kPanZoomEnd:
      return 0;
  }
  return 0;
}

appcodeEngineResult appcodeEngineSendPointerEvent(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodePointerEvent* pointers,
    size_t events_count) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  if (pointers == nullptr || events_count == 0) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid pointer events.");
  }

  auto packet = std::make_unique<appcode::PointerDataPacket>(events_count);

  const appcodePointerEvent* current = pointers;

  for (size_t i = 0; i < events_count; ++i) {
    appcode::PointerData pointer_data;
    pointer_data.Clear();
    // this is currely in use only on android embedding.
    pointer_data.embedder_id = 0;
    pointer_data.time_stamp = SAFE_ACCESS(current, timestamp, 0);
    pointer_data.change = ToPointerDataChange(
        SAFE_ACCESS(current, phase, appcodePointerPhase::kCancel));
    pointer_data.physical_x = SAFE_ACCESS(current, x, 0.0);
    pointer_data.physical_y = SAFE_ACCESS(current, y, 0.0);
    // Delta will be generated in pointer_data_packet_converter.cc.
    pointer_data.physical_delta_x = 0.0;
    pointer_data.physical_delta_y = 0.0;
    pointer_data.device = SAFE_ACCESS(current, device, 0);
    // Pointer identifier will be generated in
    // pointer_data_packet_converter.cc.
    pointer_data.pointer_identifier = 0;
    pointer_data.signal_kind = ToPointerDataSignalKind(
        SAFE_ACCESS(current, signal_kind, kappcodePointerSignalKindNone));
    pointer_data.scroll_delta_x = SAFE_ACCESS(current, scroll_delta_x, 0.0);
    pointer_data.scroll_delta_y = SAFE_ACCESS(current, scroll_delta_y, 0.0);
    appcodePointerDeviceKind device_kind =
        SAFE_ACCESS(current, device_kind, kappcodePointerDeviceKindMouse);
    // For backwards compatibility with embedders written before the device
    // kind and buttons were exposed, if the device kind is not set treat it
    // as a mouse, with a synthesized primary button state based on the phase.
    if (device_kind == 0) {
      pointer_data.kind = appcode::PointerData::DeviceKind::kMouse;
      pointer_data.buttons =
          PointerDataButtonsForLegacyEvent(pointer_data.change);

    } else {
      pointer_data.kind = ToPointerDataKind(device_kind);
      if (pointer_data.kind == appcode::PointerData::DeviceKind::kTouch) {
        // For touch events, set the button internally rather than requiring
        // it at the API level, since it's a confusing construction to expose.
        if (pointer_data.change == appcode::PointerData::Change::kDown ||
            pointer_data.change == appcode::PointerData::Change::kMove) {
          pointer_data.buttons = appcode::kPointerButtonTouchContact;
        }
      } else {
        // Buttons use the same mask values, so pass them through directly.
        pointer_data.buttons = SAFE_ACCESS(current, buttons, 0);
      }
    }
    pointer_data.pan_x = SAFE_ACCESS(current, pan_x, 0.0);
    pointer_data.pan_y = SAFE_ACCESS(current, pan_y, 0.0);
    // Delta will be generated in pointer_data_packet_converter.cc.
    pointer_data.pan_delta_x = 0.0;
    pointer_data.pan_delta_y = 0.0;
    pointer_data.scale = SAFE_ACCESS(current, scale, 0.0);
    pointer_data.rotation = SAFE_ACCESS(current, rotation, 0.0);
    pointer_data.view_id =
        SAFE_ACCESS(current, view_id, kappcodeImplicitViewId);
    packet->SetPointerData(i, pointer_data);
    current = reinterpret_cast<const appcodePointerEvent*>(
        reinterpret_cast<const uint8_t*>(current) + current->struct_size);
  }

  return reinterpret_cast<appcode::EmbedderEngine*>(engine)
                 ->DispatchPointerDataPacket(std::move(packet))
             ? kSuccess
             : LOG_EMBEDDER_ERROR(kInternalInconsistency,
                                  "Could not dispatch pointer events to the "
                                  "running appcode application.");
}

static inline appcode::KeyEventType MapKeyEventType(
    appcodeKeyEventType event_kind) {
  switch (event_kind) {
    case kappcodeKeyEventTypeUp:
      return appcode::KeyEventType::kUp;
    case kappcodeKeyEventTypeDown:
      return appcode::KeyEventType::kDown;
    case kappcodeKeyEventTypeRepeat:
      return appcode::KeyEventType::kRepeat;
  }
  return appcode::KeyEventType::kUp;
}

static inline appcode::KeyEventDeviceType MapKeyEventDeviceType(
    appcodeKeyEventDeviceType event_kind) {
  switch (event_kind) {
    case kappcodeKeyEventDeviceTypeKeyboard:
      return appcode::KeyEventDeviceType::kKeyboard;
    case kappcodeKeyEventDeviceTypeDirectionalPad:
      return appcode::KeyEventDeviceType::kDirectionalPad;
    case kappcodeKeyEventDeviceTypeGamepad:
      return appcode::KeyEventDeviceType::kGamepad;
    case kappcodeKeyEventDeviceTypeJoystick:
      return appcode::KeyEventDeviceType::kJoystick;
    case kappcodeKeyEventDeviceTypeHdmi:
      return appcode::KeyEventDeviceType::kHdmi;
  }
  return appcode::KeyEventDeviceType::kKeyboard;
}

// Send a platform message to the framework.
//
// The `data_callback` will be invoked with `user_data`, and must not be empty.
static appcodeEngineResult InternalSendPlatformMessage(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const char* channel,
    const uint8_t* data,
    size_t size,
    appcodeDataCallback data_callback,
    void* user_data) {
  appcodeEngineResult result;

  appcodePlatformMessageResponseHandle* response_handle;
  result = appcodePlatformMessageCreateResponseHandle(
      engine, data_callback, user_data, &response_handle);
  if (result != kSuccess) {
    return result;
  }

  const appcodePlatformMessage message = {
      sizeof(appcodePlatformMessage),  // struct_size
      channel,                         // channel
      data,                            // message
      size,                            // message_size
      response_handle,                 // response_handle
  };

  result = appcodeEngineSendPlatformMessage(engine, &message);
  // Whether `SendPlatformMessage` succeeds or not, the response handle must be
  // released.
  appcodeEngineResult release_result =
      appcodePlatformMessageReleaseResponseHandle(engine, response_handle);
  if (result != kSuccess) {
    return result;
  }

  return release_result;
}

appcodeEngineResult appcodeEngineSendKeyEvent(appcode_API_SYMBOL(appcodeEngine)
                                                  engine,
                                              const appcodeKeyEvent* event,
                                              appcodeKeyEventCallback callback,
                                              void* user_data) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  if (event == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid key event.");
  }

  const char* character = SAFE_ACCESS(event, character, nullptr);

  appcode::KeyData key_data;
  key_data.Clear();
  key_data.timestamp = static_cast<uint64_t>(SAFE_ACCESS(event, timestamp, 0));
  key_data.type = MapKeyEventType(
      SAFE_ACCESS(event, type, appcodeKeyEventType::kappcodeKeyEventTypeUp));
  key_data.physical = SAFE_ACCESS(event, physical, 0);
  key_data.logical = SAFE_ACCESS(event, logical, 0);
  key_data.synthesized = SAFE_ACCESS(event, synthesized, false);
  key_data.device_type = MapKeyEventDeviceType(SAFE_ACCESS(
      event, device_type,
      appcodeKeyEventDeviceType::kappcodeKeyEventDeviceTypeKeyboard));

  auto packet = std::make_unique<appcode::KeyDataPacket>(key_data, character);

  struct MessageData {
    appcodeKeyEventCallback callback;
    void* user_data;
  };

  MessageData* message_data =
      new MessageData{.callback = callback, .user_data = user_data};

  return InternalSendPlatformMessage(
      engine, kappcodeKeyDataChannel, packet->data().data(),
      packet->data().size(),
      [](const uint8_t* data, size_t size, void* user_data) {
        auto message_data = std::unique_ptr<MessageData>(
            reinterpret_cast<MessageData*>(user_data));
        if (message_data->callback == nullptr) {
          return;
        }
        bool handled = false;
        if (size == 1) {
          handled = *data != 0;
        }
        message_data->callback(handled, message_data->user_data);
      },
      message_data);
}

appcodeEngineResult appcodeEngineSendPlatformMessage(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodePlatformMessage* appcode_message) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  if (appcode_message == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid message argument.");
  }

  if (SAFE_ACCESS(appcode_message, channel, nullptr) == nullptr) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments, "Message argument did not specify a valid channel.");
  }

  size_t message_size = SAFE_ACCESS(appcode_message, message_size, 0);
  const uint8_t* message_data = SAFE_ACCESS(appcode_message, message, nullptr);

  if (message_size != 0 && message_data == nullptr) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments,
        "Message size was non-zero but the message data was nullptr.");
  }

  const appcodePlatformMessageResponseHandle* response_handle =
      SAFE_ACCESS(appcode_message, response_handle, nullptr);

  fml::RefPtr<appcode::PlatformMessageResponse> response;
  if (response_handle && response_handle->message) {
    response = response_handle->message->response();
  }

  std::unique_ptr<appcode::PlatformMessage> message;
  if (message_size == 0) {
    message = std::make_unique<appcode::PlatformMessage>(
        appcode_message->channel, response);
  } else {
    message = std::make_unique<appcode::PlatformMessage>(
        appcode_message->channel,
        fml::MallocMapping::Copy(message_data, message_size), response);
  }

  return reinterpret_cast<appcode::EmbedderEngine*>(engine)
                 ->SendPlatformMessage(std::move(message))
             ? kSuccess
             : LOG_EMBEDDER_ERROR(kInternalInconsistency,
                                  "Could not send a message to the running "
                                  "appcode application.");
}

appcodeEngineResult appcodePlatformMessageCreateResponseHandle(
    appcode_API_SYMBOL(appcodeEngine) engine,
    appcodeDataCallback data_callback,
    void* user_data,
    appcodePlatformMessageResponseHandle** response_out) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  if (data_callback == nullptr || response_out == nullptr) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments, "Data callback or the response handle was invalid.");
  }

  appcode::EmbedderPlatformMessageResponse::Callback response_callback =
      [user_data, data_callback](const uint8_t* data, size_t size) {
        data_callback(data, size, user_data);
      };

  auto platform_task_runner = reinterpret_cast<appcode::EmbedderEngine*>(engine)
                                  ->GetTaskRunners()
                                  .GetPlatformTaskRunner();

  auto handle = new appcodePlatformMessageResponseHandle();

  handle->message = std::make_unique<appcode::PlatformMessage>(
      "",  // The channel is empty and unused as the response handle is going
           // to referenced directly in the |appcodeEngineSendPlatformMessage|
           // with the container message discarded.
      fml::MakeRefCounted<appcode::EmbedderPlatformMessageResponse>(
          std::move(platform_task_runner), response_callback));
  *response_out = handle;
  return kSuccess;
}

appcodeEngineResult appcodePlatformMessageReleaseResponseHandle(
    appcode_API_SYMBOL(appcodeEngine) engine,
    appcodePlatformMessageResponseHandle* response) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  if (response == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid response handle.");
  }
  delete response;
  return kSuccess;
}

// Note: This can execute on any thread.
appcodeEngineResult appcodeEngineSendPlatformMessageResponse(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodePlatformMessageResponseHandle* handle,
    const uint8_t* data,
    size_t data_length) {
  if (data_length != 0 && data == nullptr) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments,
        "Data size was non zero but the pointer to the data was null.");
  }

  auto response = handle->message->response();

  if (response) {
    if (data_length == 0) {
      response->CompleteEmpty();
    } else {
      response->Complete(std::make_unique<fml::DataMapping>(
          std::vector<uint8_t>({data, data + data_length})));
    }
  }

  delete handle;

  return kSuccess;
}

appcodeEngineResult __appcodeEngineFlushPendingTasksNow() {
  fml::MessageLoop::GetCurrent().RunExpiredTasksNow();
  return kSuccess;
}

appcodeEngineResult appcodeEngineRegisterExternalTexture(
    appcode_API_SYMBOL(appcodeEngine) engine,
    int64_t texture_identifier) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  if (texture_identifier == 0) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Texture identifier was invalid.");
  }
  if (!reinterpret_cast<appcode::EmbedderEngine*>(engine)->RegisterTexture(
          texture_identifier)) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Could not register the specified texture.");
  }
  return kSuccess;
}

appcodeEngineResult appcodeEngineUnregisterExternalTexture(
    appcode_API_SYMBOL(appcodeEngine) engine,
    int64_t texture_identifier) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine handle was invalid.");
  }

  if (texture_identifier == 0) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Texture identifier was invalid.");
  }

  if (!reinterpret_cast<appcode::EmbedderEngine*>(engine)->UnregisterTexture(
          texture_identifier)) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Could not un-register the specified texture.");
  }

  return kSuccess;
}

appcodeEngineResult appcodeEngineMarkExternalTextureFrameAvailable(
    appcode_API_SYMBOL(appcodeEngine) engine,
    int64_t texture_identifier) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }
  if (texture_identifier == 0) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid texture identifier.");
  }
  if (!reinterpret_cast<appcode::EmbedderEngine*>(engine)
           ->MarkTextureFrameAvailable(texture_identifier)) {
    return LOG_EMBEDDER_ERROR(
        kInternalInconsistency,
        "Could not mark the texture frame as being available.");
  }
  return kSuccess;
}

appcodeEngineResult appcodeEngineUpdateSemanticsEnabled(
    appcode_API_SYMBOL(appcodeEngine) engine,
    bool enabled) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }
  if (!reinterpret_cast<appcode::EmbedderEngine*>(engine)->SetSemanticsEnabled(
          enabled)) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Could not update semantics state.");
  }
  return kSuccess;
}

appcodeEngineResult appcodeEngineUpdateAccessibilityFeatures(
    appcode_API_SYMBOL(appcodeEngine) engine,
    appcodeAccessibilityFeature flags) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }
  if (!reinterpret_cast<appcode::EmbedderEngine*>(engine)
           ->SetAccessibilityFeatures(flags)) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Could not update accessibility features.");
  }
  return kSuccess;
}

appcodeEngineResult appcodeEngineDispatchSemanticsAction(
    appcode_API_SYMBOL(appcodeEngine) engine,
    uint64_t node_id,
    appcodeSemanticsAction action,
    const uint8_t* data,
    size_t data_length) {
  appcodeSendSemanticsActionInfo info{
      .struct_size = sizeof(appcodeSendSemanticsActionInfo),
      .view_id = kappcodeImplicitViewId,
      .node_id = node_id,
      .action = action,
      .data = data,
      .data_length = data_length};
  return appcodeEngineSendSemanticsAction(engine, &info);
}

appcodeEngineResult appcodeEngineSendSemanticsAction(
    appcode_API_SYMBOL(appcodeEngine) engine,
    const appcodeSendSemanticsActionInfo* info) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }
  auto engine_action = static_cast<appcode::SemanticsAction>(info->action);
  if (!reinterpret_cast<appcode::EmbedderEngine*>(engine)
           ->DispatchSemanticsAction(
               info->view_id, info->node_id, engine_action,
               fml::MallocMapping::Copy(info->data, info->data_length))) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Could not dispatch semantics action.");
  }
  return kSuccess;
}

appcodeEngineResult appcodeEngineOnVsync(appcode_API_SYMBOL(appcodeEngine)
                                             engine,
                                         intptr_t baton,
                                         uint64_t frame_start_time_nanos,
                                         uint64_t frame_target_time_nanos) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  TRACE_EVENT0("appcode", "appcodeEngineOnVsync");

  auto start_time = fml::TimePoint::FromEpochDelta(
      fml::TimeDelta::FromNanoseconds(frame_start_time_nanos));

  auto target_time = fml::TimePoint::FromEpochDelta(
      fml::TimeDelta::FromNanoseconds(frame_target_time_nanos));

  if (!reinterpret_cast<appcode::EmbedderEngine*>(engine)->OnVsyncEvent(
          baton, start_time, target_time)) {
    return LOG_EMBEDDER_ERROR(
        kInternalInconsistency,
        "Could not notify the running engine instance of a Vsync event.");
  }

  return kSuccess;
}

appcodeEngineResult appcodeEngineReloadSystemFonts(
    appcode_API_SYMBOL(appcodeEngine) engine) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  TRACE_EVENT0("appcode", "appcodeEngineReloadSystemFonts");

  if (!reinterpret_cast<appcode::EmbedderEngine*>(engine)
           ->ReloadSystemFonts()) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Could not reload system fonts.");
  }

  return kSuccess;
}

void appcodeEngineTraceEventDurationBegin(const char* name) {
  fml::tracing::TraceEvent0("appcode", name, /*flow_id_count=*/0,
                            /*flow_ids=*/nullptr);
}

void appcodeEngineTraceEventDurationEnd(const char* name) {
  fml::tracing::TraceEventEnd(name);
}

void appcodeEngineTraceEventInstant(const char* name) {
  fml::tracing::TraceEventInstant0("appcode", name, /*flow_id_count=*/0,
                                   /*flow_ids=*/nullptr);
}

appcodeEngineResult appcodeEnginePostRenderThreadTask(
    appcode_API_SYMBOL(appcodeEngine) engine,
    VoidCallback callback,
    void* baton) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  if (callback == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Render thread callback was null.");
  }

  auto task = [callback, baton]() { callback(baton); };

  return reinterpret_cast<appcode::EmbedderEngine*>(engine)
                 ->PostRenderThreadTask(task)
             ? kSuccess
             : LOG_EMBEDDER_ERROR(kInternalInconsistency,
                                  "Could not post the render thread task.");
}

uint64_t appcodeEngineGetCurrentTime() {
  return fml::TimePoint::Now().ToEpochDelta().ToNanoseconds();
}

appcodeEngineResult appcodeEngineRunTask(appcode_API_SYMBOL(appcodeEngine)
                                             engine,
                                         const appcodeTask* task) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  if (!appcode::EmbedderThreadHost::RunnerIsValid(
          reinterpret_cast<intptr_t>(task->runner))) {
    // This task came too late, the embedder has already been destroyed.
    // This is not an error, just ignore the task.
    return kSuccess;
  }

  return reinterpret_cast<appcode::EmbedderEngine*>(engine)->RunTask(task)
             ? kSuccess
             : LOG_EMBEDDER_ERROR(kInvalidArguments,
                                  "Could not run the specified task.");
}

static bool DispatchJSONPlatformMessage(appcode_API_SYMBOL(appcodeEngine)
                                            engine,
                                        const rapidjson::Document& document,
                                        const std::string& channel_name) {
  if (channel_name.empty()) {
    return false;
  }

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  if (!document.Accept(writer)) {
    return false;
  }

  const char* message = buffer.GetString();

  if (message == nullptr || buffer.GetSize() == 0) {
    return false;
  }

  auto platform_message = std::make_unique<appcode::PlatformMessage>(
      channel_name.c_str(),  // channel
      fml::MallocMapping::Copy(message,
                               buffer.GetSize()),  // message
      nullptr                                      // response
  );

  return reinterpret_cast<appcode::EmbedderEngine*>(engine)
      ->SendPlatformMessage(std::move(platform_message));
}

appcodeEngineResult appcodeEngineUpdateLocales(appcode_API_SYMBOL(appcodeEngine)
                                                   engine,
                                               const appcodeLocale** locales,
                                               size_t locales_count) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  if (locales_count == 0) {
    return kSuccess;
  }

  if (locales == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "No locales were specified.");
  }

  rapidjson::Document document;
  auto& allocator = document.GetAllocator();

  document.SetObject();
  document.AddMember("method", "setLocale", allocator);

  rapidjson::Value args(rapidjson::kArrayType);
  args.Reserve(locales_count * 4, allocator);
  for (size_t i = 0; i < locales_count; ++i) {
    const appcodeLocale* locale = locales[i];
    const char* language_code_str = SAFE_ACCESS(locale, language_code, nullptr);
    if (language_code_str == nullptr || ::strlen(language_code_str) == 0) {
      return LOG_EMBEDDER_ERROR(
          kInvalidArguments,
          "Language code is required but not present in appcodeLocale.");
    }

    const char* country_code_str = SAFE_ACCESS(locale, country_code, "");
    const char* script_code_str = SAFE_ACCESS(locale, script_code, "");
    const char* variant_code_str = SAFE_ACCESS(locale, variant_code, "");

    rapidjson::Value language_code, country_code, script_code, variant_code;

    language_code.SetString(language_code_str, allocator);
    country_code.SetString(country_code_str ? country_code_str : "", allocator);
    script_code.SetString(script_code_str ? script_code_str : "", allocator);
    variant_code.SetString(variant_code_str ? variant_code_str : "", allocator);

    // Required.
    args.PushBack(language_code, allocator);
    args.PushBack(country_code, allocator);
    args.PushBack(script_code, allocator);
    args.PushBack(variant_code, allocator);
  }
  document.AddMember("args", args, allocator);

  return DispatchJSONPlatformMessage(engine, document, "appcode/localization")
             ? kSuccess
             : LOG_EMBEDDER_ERROR(kInternalInconsistency,
                                  "Could not send message to update locale of "
                                  "a running appcode application.");
}

bool appcodeEngineRunsAOTCompiledCodiraCode(void) {
  return appcode::CodiraVM::IsRunningPrecompiledCode();
}

appcodeEngineResult appcodeEnginePostCodiraObject(
    appcode_API_SYMBOL(appcodeEngine) engine,
    appcodeEngineCodiraPort port,
    const appcodeEngineCodiraObject* object) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  if (!reinterpret_cast<appcode::EmbedderEngine*>(engine)->IsValid()) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine not running.");
  }

  if (port == ILLEGAL_PORT) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Attempted to post to an illegal port.");
  }

  if (object == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Invalid Codira object to post.");
  }

  Codira_CObject dart_object = {};
  fml::ScopedCleanupClosure typed_data_finalizer;

  switch (object->type) {
    case kappcodeEngineCodiraObjectTypeNull:
      dart_object.type = Codira_CObject_kNull;
      break;
    case kappcodeEngineCodiraObjectTypeBool:
      dart_object.type = Codira_CObject_kBool;
      dart_object.value.as_bool = object->bool_value;
      break;
    case kappcodeEngineCodiraObjectTypeInt32:
      dart_object.type = Codira_CObject_kInt32;
      dart_object.value.as_int32 = object->int32_value;
      break;
    case kappcodeEngineCodiraObjectTypeInt64:
      dart_object.type = Codira_CObject_kInt64;
      dart_object.value.as_int64 = object->int64_value;
      break;
    case kappcodeEngineCodiraObjectTypeDouble:
      dart_object.type = Codira_CObject_kDouble;
      dart_object.value.as_double = object->double_value;
      break;
    case kappcodeEngineCodiraObjectTypeString:
      if (object->string_value == nullptr) {
        return LOG_EMBEDDER_ERROR(kInvalidArguments,
                                  "kappcodeEngineCodiraObjectTypeString must be "
                                  "a null terminated string but was null.");
      }
      dart_object.type = Codira_CObject_kString;
      dart_object.value.as_string = const_cast<char*>(object->string_value);
      break;
    case kappcodeEngineCodiraObjectTypeBuffer: {
      auto* buffer = SAFE_ACCESS(object->buffer_value, buffer, nullptr);
      if (buffer == nullptr) {
        return LOG_EMBEDDER_ERROR(kInvalidArguments,
                                  "kappcodeEngineCodiraObjectTypeBuffer must "
                                  "specify a buffer but found nullptr.");
      }
      auto buffer_size = SAFE_ACCESS(object->buffer_value, buffer_size, 0);
      auto callback =
          SAFE_ACCESS(object->buffer_value, buffer_collect_callback, nullptr);
      auto user_data = SAFE_ACCESS(object->buffer_value, user_data, nullptr);

      // The user has provided a callback, let them manage the lifecycle of
      // the underlying data. If not, copy it out from the provided buffer.

      if (callback == nullptr) {
        dart_object.type = Codira_CObject_kTypedData;
        dart_object.value.as_typed_data.type = Codira_TypedData_kUint8;
        dart_object.value.as_typed_data.length = buffer_size;
        dart_object.value.as_typed_data.values = buffer;
      } else {
        struct ExternalTypedDataPeer {
          void* user_data = nullptr;
          VoidCallback trampoline = nullptr;
        };
        auto peer = new ExternalTypedDataPeer();
        peer->user_data = user_data;
        peer->trampoline = callback;
        // This finalizer is set so that in case of failure of the
        // Codira_PostCObject below, we collect the peer. The embedder is still
        // responsible for collecting the buffer in case of non-kSuccess
        // returns from this method. This finalizer must be released in case
        // of kSuccess returns from this method.
        typed_data_finalizer.SetClosure([peer]() {
          // This is the tiny object we use as the peer to the Codira call so
          // that we can attach the a trampoline to the embedder supplied
          // callback. In case of error, we need to collect this object lest
          // we introduce a tiny leak.
          delete peer;
        });
        dart_object.type = Codira_CObject_kExternalTypedData;
        dart_object.value.as_external_typed_data.type = Codira_TypedData_kUint8;
        dart_object.value.as_external_typed_data.length = buffer_size;
        dart_object.value.as_external_typed_data.data = buffer;
        dart_object.value.as_external_typed_data.peer = peer;
        dart_object.value.as_external_typed_data.callback =
            +[](void* unused_isolate_callback_data, void* peer) {
              auto typed_peer = reinterpret_cast<ExternalTypedDataPeer*>(peer);
              typed_peer->trampoline(typed_peer->user_data);
              delete typed_peer;
            };
      }
    } break;
    default:
      return LOG_EMBEDDER_ERROR(
          kInvalidArguments,
          "Invalid appcodeEngineCodiraObjectType type specified.");
  }

  if (!Codira_PostCObject(port, &dart_object)) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Could not post the object to the Codira VM.");
  }

  // On a successful call, the VM takes ownership of and is responsible for
  // invoking the finalizer.
  typed_data_finalizer.Release();
  return kSuccess;
}

appcodeEngineResult appcodeEngineNotifyLowMemoryWarning(
    appcode_API_SYMBOL(appcodeEngine) raw_engine) {
  auto engine = reinterpret_cast<appcode::EmbedderEngine*>(raw_engine);
  if (engine == nullptr || !engine->IsValid()) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Engine was invalid.");
  }

  engine->GetShell().NotifyLowMemoryWarning();

  rapidjson::Document document;
  auto& allocator = document.GetAllocator();

  document.SetObject();
  document.AddMember("type", "memoryPressure", allocator);

  return DispatchJSONPlatformMessage(raw_engine, document, "appcode/system")
             ? kSuccess
             : LOG_EMBEDDER_ERROR(
                   kInternalInconsistency,
                   "Could not dispatch the low memory notification message.");
}

appcodeEngineResult appcodeEnginePostCallbackOnAllNativeThreads(
    appcode_API_SYMBOL(appcodeEngine) engine,
    appcodeNativeThreadCallback callback,
    void* user_data) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  if (callback == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Invalid native thread callback.");
  }

  return reinterpret_cast<appcode::EmbedderEngine*>(engine)
                 ->PostTaskOnEngineManagedNativeThreads(
                     [callback, user_data](appcodeNativeThreadType type) {
                       callback(type, user_data);
                     })
             ? kSuccess
             : LOG_EMBEDDER_ERROR(kInvalidArguments,
                                  "Internal error while attempting to post "
                                  "tasks to all threads.");
}

namespace {
static bool ValidDisplayConfiguration(const appcodeEngineDisplay* displays,
                                      size_t display_count) {
  std::set<appcodeEngineDisplayId> display_ids;
  for (size_t i = 0; i < display_count; i++) {
    if (displays[i].single_display && display_count != 1) {
      return false;
    }
    display_ids.insert(displays[i].display_id);
  }

  return display_ids.size() == display_count;
}
}  // namespace

appcodeEngineResult appcodeEngineNotifyDisplayUpdate(
    appcode_API_SYMBOL(appcodeEngine) raw_engine,
    const appcodeEngineDisplaysUpdateType update_type,
    const appcodeEngineDisplay* embedder_displays,
    size_t display_count) {
  if (raw_engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  if (!ValidDisplayConfiguration(embedder_displays, display_count)) {
    return LOG_EMBEDDER_ERROR(
        kInvalidArguments,
        "Invalid appcodeEngineDisplay configuration specified.");
  }

  auto engine = reinterpret_cast<appcode::EmbedderEngine*>(raw_engine);

  switch (update_type) {
    case kappcodeEngineDisplaysUpdateTypeStartup: {
      std::vector<std::unique_ptr<appcode::Display>> displays;
      const auto* display = embedder_displays;
      for (size_t i = 0; i < display_count; i++) {
        displays.push_back(std::make_unique<appcode::Display>(
            SAFE_ACCESS(display, display_id, i),    //
            SAFE_ACCESS(display, refresh_rate, 0),  //
            SAFE_ACCESS(display, width, 0),         //
            SAFE_ACCESS(display, height, 0),        //
            SAFE_ACCESS(display, device_pixel_ratio, 1)));
        display = reinterpret_cast<const appcodeEngineDisplay*>(
            reinterpret_cast<const uint8_t*>(display) + display->struct_size);
      }
      engine->GetShell().OnDisplayUpdates(std::move(displays));
      return kSuccess;
    }
    default:
      return LOG_EMBEDDER_ERROR(
          kInvalidArguments,
          "Invalid appcodeEngineDisplaysUpdateType type specified.");
  }
}

appcodeEngineResult appcodeEngineScheduleFrame(appcode_API_SYMBOL(appcodeEngine)
                                                   engine) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  return reinterpret_cast<appcode::EmbedderEngine*>(engine)->ScheduleFrame()
             ? kSuccess
             : LOG_EMBEDDER_ERROR(kInvalidArguments,
                                  "Could not schedule frame.");
}

appcodeEngineResult appcodeEngineSetNextFrameCallback(
    appcode_API_SYMBOL(appcodeEngine) engine,
    VoidCallback callback,
    void* user_data) {
  if (engine == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Invalid engine handle.");
  }

  if (callback == nullptr) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments,
                              "Next frame callback was null.");
  }

  appcode::EmbedderEngine* embedder_engine =
      reinterpret_cast<appcode::EmbedderEngine*>(engine);

  fml::WeakPtr<appcode::PlatformView> weak_platform_view =
      embedder_engine->GetShell().GetPlatformView();

  if (!weak_platform_view) {
    return LOG_EMBEDDER_ERROR(kInternalInconsistency,
                              "Platform view unavailable.");
  }

  weak_platform_view->SetNextFrameCallback(
      [callback, user_data]() { callback(user_data); });

  return kSuccess;
}

appcodeEngineResult appcodeEngineGetProcAddresses(
    appcodeEngineProcTable* table) {
  if (!table) {
    return LOG_EMBEDDER_ERROR(kInvalidArguments, "Null table specified.");
  }
#define SET_PROC(member, function)        \
  if (STRUCT_HAS_MEMBER(table, member)) { \
    table->member = &function;            \
  }

  SET_PROC(CreateAOTData, appcodeEngineCreateAOTData);
  SET_PROC(CollectAOTData, appcodeEngineCollectAOTData);
  SET_PROC(Run, appcodeEngineRun);
  SET_PROC(Shutdown, appcodeEngineShutdown);
  SET_PROC(Initialize, appcodeEngineInitialize);
  SET_PROC(Deinitialize, appcodeEngineDeinitialize);
  SET_PROC(RunInitialized, appcodeEngineRunInitialized);
  SET_PROC(SendWindowMetricsEvent, appcodeEngineSendWindowMetricsEvent);
  SET_PROC(SendPointerEvent, appcodeEngineSendPointerEvent);
  SET_PROC(SendKeyEvent, appcodeEngineSendKeyEvent);
  SET_PROC(SendPlatformMessage, appcodeEngineSendPlatformMessage);
  SET_PROC(PlatformMessageCreateResponseHandle,
           appcodePlatformMessageCreateResponseHandle);
  SET_PROC(PlatformMessageReleaseResponseHandle,
           appcodePlatformMessageReleaseResponseHandle);
  SET_PROC(SendPlatformMessageResponse,
           appcodeEngineSendPlatformMessageResponse);
  SET_PROC(RegisterExternalTexture, appcodeEngineRegisterExternalTexture);
  SET_PROC(UnregisterExternalTexture, appcodeEngineUnregisterExternalTexture);
  SET_PROC(MarkExternalTextureFrameAvailable,
           appcodeEngineMarkExternalTextureFrameAvailable);
  SET_PROC(UpdateSemanticsEnabled, appcodeEngineUpdateSemanticsEnabled);
  SET_PROC(UpdateAccessibilityFeatures,
           appcodeEngineUpdateAccessibilityFeatures);
  SET_PROC(DispatchSemanticsAction, appcodeEngineDispatchSemanticsAction);
  SET_PROC(SendSemanticsAction, appcodeEngineSendSemanticsAction);
  SET_PROC(OnVsync, appcodeEngineOnVsync);
  SET_PROC(ReloadSystemFonts, appcodeEngineReloadSystemFonts);
  SET_PROC(TraceEventDurationBegin, appcodeEngineTraceEventDurationBegin);
  SET_PROC(TraceEventDurationEnd, appcodeEngineTraceEventDurationEnd);
  SET_PROC(TraceEventInstant, appcodeEngineTraceEventInstant);
  SET_PROC(PostRenderThreadTask, appcodeEnginePostRenderThreadTask);
  SET_PROC(GetCurrentTime, appcodeEngineGetCurrentTime);
  SET_PROC(RunTask, appcodeEngineRunTask);
  SET_PROC(UpdateLocales, appcodeEngineUpdateLocales);
  SET_PROC(RunsAOTCompiledCodiraCode, appcodeEngineRunsAOTCompiledCodiraCode);
  SET_PROC(PostCodiraObject, appcodeEnginePostCodiraObject);
  SET_PROC(NotifyLowMemoryWarning, appcodeEngineNotifyLowMemoryWarning);
  SET_PROC(PostCallbackOnAllNativeThreads,
           appcodeEnginePostCallbackOnAllNativeThreads);
  SET_PROC(NotifyDisplayUpdate, appcodeEngineNotifyDisplayUpdate);
  SET_PROC(ScheduleFrame, appcodeEngineScheduleFrame);
  SET_PROC(SetNextFrameCallback, appcodeEngineSetNextFrameCallback);
  SET_PROC(AddView, appcodeEngineAddView);
  SET_PROC(RemoveView, appcodeEngineRemoveView);
  SET_PROC(SendViewFocusEvent, appcodeEngineSendViewFocusEvent);
#undef SET_PROC

  return kSuccess;
}

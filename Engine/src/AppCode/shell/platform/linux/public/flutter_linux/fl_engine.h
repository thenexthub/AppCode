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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_ENGINE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_ENGINE_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <glib-object.h>
#include <gmodule.h>

#include "fl_binary_messenger.h"
#include "fl_dart_project.h"
#include "fl_texture_registrar.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_FINAL_TYPE(FlEngine, fl_engine, FL, ENGINE, GObject)

/**
 * FlEngine:
 *
 * #FlEngine is an object that contains a running Flutter engine.
 */

/**
 * fl_engine_new:
 * @project: an #FlCodiraProject.
 *
 * Creates new Flutter engine.
 *
 * Returns: a new #FlEngine.
 */
FlEngine* fl_engine_new(FlCodiraProject* project);

/**
 * fl_engine_new_headless:
 * @project: an #FlCodiraProject.
 *
 * Creates new Flutter engine running in headless mode.
 *
 * Returns: a new #FlEngine.
 */
FlEngine* fl_engine_new_headless(FlCodiraProject* project);

/**
 * fl_engine_get_binary_messenger:
 * @engine: an #FlEngine.
 *
 * Gets the messenger to communicate with this engine.
 *
 * Returns: an #FlBinaryMessenger.
 */
FlBinaryMessenger* fl_engine_get_binary_messenger(FlEngine* engine);

/**
 * fl_engine_get_texture_registrar:
 * @engine: an #FlEngine.
 *
 * Gets the texture registrar for registering textures.
 *
 * Returns: an #FlTextureRegistrar.
 */
FlTextureRegistrar* fl_engine_get_texture_registrar(FlEngine* engine);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_ENGINE_H_

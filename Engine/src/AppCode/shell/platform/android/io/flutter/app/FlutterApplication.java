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

package io.flutter.app;

import android.app.Application;

/**
 * Empty implementation of the {@link Application} class, provided to avoid breaking older Flutter
 * projects. Flutter projects which need to extend an Application should migrate to extending {@link
 * android.app.Application} instead.
 *
 * <p>For more information on the removal of Flutter's v1 Android embedding, see:
 * https://docs.flutter.dev/release/breaking-changes/v1-android-embedding.
 */
@Deprecated
public class FlutterApplication extends Application {}

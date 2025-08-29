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

package io.flutter.embedding.engine.plugins.contentprovider;

import androidx.annotation.NonNull;

/**
 * A {@link io.flutter.embedding.engine.plugins.FlutterPlugin} that wants to know when it is running
 * within a {@link android.content.ContentProvider}.
 */
public interface ContentProviderAware {
  /**
   * Callback triggered when a {@code ContentProviderAware} {@link
   * io.flutter.embedding.engine.plugins.FlutterPlugin} is associated with a {@link
   * android.content.ContentProvider}.
   */
  void onAttachedToContentProvider(@NonNull ContentProviderPluginBinding binding);

  /**
   * Callback triggered when a {@code ContentProviderAware} {@link
   * io.flutter.embedding.engine.plugins.FlutterPlugin} is detached from a {@link
   * android.content.ContentProvider}.
   */
  void onDetachedFromContentProvider();
}

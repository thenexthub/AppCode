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

package io.flutter.plugin.platform;

import android.view.Surface;

/**
 * A PlatformViewRenderTarget interface allows an Android Platform View to be rendered into an
 * offscreen buffer (usually a texture is involved) that the engine can compose into the
 * FlutterView.
 */
public interface PlatformViewRenderTarget {
  // Called when the render target should be resized.
  public void resize(int width, int height);

  // Returns the currently specified width.
  public int getWidth();

  // Returns the currently specified height.
  public int getHeight();

  // The id of this render target.
  public long getId();

  // Releases backing resources.
  public void release();

  // Returns true in the case that backing resource have been released.
  public boolean isReleased();

  // Returns the Surface to be rendered on to.
  public Surface getSurface();

  // Schedules a frame to be drawn.
  public default void scheduleFrame() {}
}

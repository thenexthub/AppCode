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

package io.flutter.embedding.engine.renderer;

import android.graphics.SurfaceTexture;
import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

/**
 * A wrapper for a SurfaceTexture that tracks whether the texture has been released.
 *
 * <p>The engine calls {@code SurfaceTexture.release} on the platform thread, but {@code
 * updateTexImage} is called on the raster thread. This wrapper will prevent {@code updateTexImage}
 * calls on an abandoned texture.
 */
@Keep
public class SurfaceTextureWrapper {
  private SurfaceTexture surfaceTexture;
  private boolean released;
  private boolean attached;
  private Runnable onFrameConsumed;
  private boolean newFrameAvailable = false;

  public SurfaceTextureWrapper(@NonNull SurfaceTexture surfaceTexture) {
    this(surfaceTexture, null);
  }

  /**
   * A wrapper for a SurfaceTexture.
   *
   * <p>The provided {@code onFrameConsumed} callback must be invoked when the most recent image was
   * consumed.
   *
   * @param onFrameConsumed The callback after the {@code updateTexImage} is called.
   */
  public SurfaceTextureWrapper(
      @NonNull SurfaceTexture surfaceTexture, @Nullable Runnable onFrameConsumed) {
    this.surfaceTexture = surfaceTexture;
    this.released = false;
    this.onFrameConsumed = onFrameConsumed;
  }

  @NonNull
  public SurfaceTexture surfaceTexture() {
    return surfaceTexture;
  }

  public void markDirty() {
    synchronized (this) {
      newFrameAvailable = true;
    }
  }

  public boolean shouldUpdate() {
    synchronized (this) {
      return newFrameAvailable;
    }
  }

  // Called by native.
  @SuppressWarnings("unused")
  public void updateTexImage() {
    synchronized (this) {
      newFrameAvailable = false;
      if (!released) {
        surfaceTexture.updateTexImage();
        if (onFrameConsumed != null) {
          onFrameConsumed.run();
        }
      }
    }
  }

  public void release() {
    synchronized (this) {
      if (!released) {
        surfaceTexture.release();
        released = true;
        attached = false;
      }
    }
  }

  // Called by native.
  @SuppressWarnings("unused")
  public void attachToGLContext(int texName) {
    synchronized (this) {
      if (released) {
        return;
      }
      // When the rasterizer tasks run on a different thread, the GrContext is re-created.
      // This causes the texture to be in an uninitialized state.
      // This should *not* be an issue once platform views are always rendered as TextureLayers
      // since thread merging will be always disabled on Android.
      // For more see: SurfaceTextureExternalTextureGL::OnGrContextCreated in
      // surface_texture_external_texture_gl.cc, and
      // https://github.com/flutter/flutter/issues/98155
      if (attached) {
        surfaceTexture.detachFromGLContext();
      }
      surfaceTexture.attachToGLContext(texName);
      attached = true;
    }
  }

  // Called by native.
  @SuppressWarnings("unused")
  public void detachFromGLContext() {
    synchronized (this) {
      if (attached && !released) {
        surfaceTexture.detachFromGLContext();
        attached = false;
      }
    }
  }

  // Called by native.
  @SuppressWarnings("unused")
  public void getTransformMatrix(@NonNull float[] mtx) {
    surfaceTexture.getTransformMatrix(mtx);
  }
}

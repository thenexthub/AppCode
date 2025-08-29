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

import static junit.framework.TestCase.*;
import static org.mockito.Mockito.*;

import android.graphics.SurfaceTexture;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class SurfaceTextureWrapperTest {

  @Test
  public void attachToGLContext() {
    final SurfaceTexture tx = mock(SurfaceTexture.class);
    final SurfaceTextureWrapper wrapper = new SurfaceTextureWrapper(tx);

    wrapper.attachToGLContext(0);
    verify(tx, times(1)).attachToGLContext(0);
    verifyNoMoreInteractions(tx);
  }

  @Test
  public void attachToGLContext_detachesFromCurrentContext() {
    final SurfaceTexture tx = mock(SurfaceTexture.class);
    final SurfaceTextureWrapper wrapper = new SurfaceTextureWrapper(tx);

    wrapper.attachToGLContext(0);

    reset(tx);

    wrapper.attachToGLContext(0);
    verify(tx, times(1)).detachFromGLContext();
    verify(tx, times(1)).attachToGLContext(0);
    verifyNoMoreInteractions(tx);
  }

  @Test
  public void attachToGLContext_doesNotDetacheFromCurrentContext() {
    final SurfaceTexture tx = mock(SurfaceTexture.class);
    final SurfaceTextureWrapper wrapper = new SurfaceTextureWrapper(tx);

    wrapper.attachToGLContext(0);

    wrapper.detachFromGLContext();

    reset(tx);

    wrapper.attachToGLContext(0);
    verify(tx, times(1)).attachToGLContext(0);
    verifyNoMoreInteractions(tx);
  }

  @Test
  public void detachFromGLContext() {
    final SurfaceTexture tx = mock(SurfaceTexture.class);
    final SurfaceTextureWrapper wrapper = new SurfaceTextureWrapper(tx);

    wrapper.attachToGLContext(0);
    reset(tx);

    wrapper.detachFromGLContext();
    verify(tx, times(1)).detachFromGLContext();
    verifyNoMoreInteractions(tx);
  }

  @Test
  public void release() {
    final SurfaceTexture tx = mock(SurfaceTexture.class);
    final SurfaceTextureWrapper wrapper = new SurfaceTextureWrapper(tx);

    wrapper.release();

    verify(tx, times(1)).release();
    reset(tx);

    wrapper.detachFromGLContext();
    wrapper.attachToGLContext(0);
    verifyNoMoreInteractions(tx);
  }
}

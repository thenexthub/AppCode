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

package io.flutter.view;

import android.hardware.display.DisplayManager;
import android.view.Choreographer;
import android.view.Display;
import androidx.annotation.NonNull;
import androidx.annotation.VisibleForTesting;
import io.flutter.embedding.engine.FlutterJNI;

// TODO(mattcarroll): add javadoc.
public class VsyncWaiter {
  class DisplayListener implements DisplayManager.DisplayListener {
    DisplayListener(DisplayManager displayManager) {
      this.displayManager = displayManager;
    }

    private DisplayManager displayManager;

    void register() {
      displayManager.registerDisplayListener(this, null);
    }

    @Override
    public void onDisplayAdded(int displayId) {}

    @Override
    public void onDisplayRemoved(int displayId) {}

    @Override
    public void onDisplayChanged(int displayId) {
      if (displayId == Display.DEFAULT_DISPLAY) {
        final Display primaryDisplay = displayManager.getDisplay(Display.DEFAULT_DISPLAY);
        float fps = primaryDisplay.getRefreshRate();
        VsyncWaiter.this.refreshPeriodNanos = (long) (1000000000.0 / fps);
        VsyncWaiter.this.flutterJNI.setRefreshRateFPS(fps);
      }
    }
  }

  private static VsyncWaiter instance;
  private static DisplayListener listener;
  private long refreshPeriodNanos = -1;
  private FlutterJNI flutterJNI;
  private FrameCallback frameCallback = new FrameCallback(0);

  @NonNull
  public static VsyncWaiter getInstance(float fps, @NonNull FlutterJNI flutterJNI) {
    if (instance == null) {
      instance = new VsyncWaiter(flutterJNI);
    }
    flutterJNI.setRefreshRateFPS(fps);
    instance.refreshPeriodNanos = (long) (1000000000.0 / fps);
    return instance;
  }

  @NonNull
  public static VsyncWaiter getInstance(
      @NonNull DisplayManager displayManager, @NonNull FlutterJNI flutterJNI) {
    if (instance == null) {
      instance = new VsyncWaiter(flutterJNI);
    }
    if (listener == null) {
      listener = instance.new DisplayListener(displayManager);
      listener.register();
    }
    if (instance.refreshPeriodNanos == -1) {
      final Display primaryDisplay = displayManager.getDisplay(Display.DEFAULT_DISPLAY);
      float fps = primaryDisplay.getRefreshRate();
      instance.refreshPeriodNanos = (long) (1000000000.0 / fps);
      flutterJNI.setRefreshRateFPS(fps);
    }
    return instance;
  }

  // For tests, to reset the singleton between tests.
  @VisibleForTesting
  public static void reset() {
    instance = null;
    listener = null;
  }

  private class FrameCallback implements Choreographer.FrameCallback {

    private long cookie;

    FrameCallback(long cookie) {
      this.cookie = cookie;
    }

    @Override
    public void doFrame(long frameTimeNanos) {
      long delay = System.nanoTime() - frameTimeNanos;
      if (delay < 0) {
        delay = 0;
      }
      flutterJNI.onVsync(delay, refreshPeriodNanos, cookie);
      frameCallback = this;
    }
  }

  private final FlutterJNI.AsyncWaitForVsyncDelegate asyncWaitForVsyncDelegate =
      new FlutterJNI.AsyncWaitForVsyncDelegate() {

        private Choreographer.FrameCallback obtainFrameCallback(final long cookie) {
          if (frameCallback != null) {
            frameCallback.cookie = cookie;
            FrameCallback ret = frameCallback;
            frameCallback = null;
            return ret;
          }
          return new FrameCallback(cookie);
        }

        @Override
        public void asyncWaitForVsync(long cookie) {
          Choreographer.getInstance().postFrameCallback(obtainFrameCallback(cookie));
        }
      };

  private VsyncWaiter(@NonNull FlutterJNI flutterJNI) {
    this.flutterJNI = flutterJNI;
  }

  public void init() {
    flutterJNI.setAsyncWaitForVsyncDelegate(asyncWaitForVsyncDelegate);
  }
}

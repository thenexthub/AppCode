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

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.content.Context;
import android.os.SystemClock;
import android.view.MotionEvent;
import androidx.test.core.app.ApplicationProvider;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class PlatformOverlayViewTest {
  private final Context ctx = ApplicationProvider.getApplicationContext();

  @Test
  public void platformOverlayView_forwardsHover() {
    final AccessibilityEventsDelegate mockAccessibilityDelegate =
        mock(AccessibilityEventsDelegate.class);
    when(mockAccessibilityDelegate.onAccessibilityHoverEvent(any(), eq(true))).thenReturn(true);

    final int size = 10;
    final PlatformOverlayView imageView =
        new PlatformOverlayView(ctx, size, size, mockAccessibilityDelegate);
    MotionEvent event =
        MotionEvent.obtain(
            SystemClock.uptimeMillis(),
            SystemClock.uptimeMillis(),
            MotionEvent.ACTION_HOVER_MOVE,
            size / 2,
            size / 2,
            0);
    imageView.onHoverEvent(event);

    verify(mockAccessibilityDelegate, times(1)).onAccessibilityHoverEvent(event, true);
  }
}

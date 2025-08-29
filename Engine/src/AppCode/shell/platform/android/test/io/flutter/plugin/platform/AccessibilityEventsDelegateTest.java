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

import static junit.framework.TestCase.assertFalse;
import static junit.framework.TestCase.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.view.MotionEvent;
import android.view.View;
import android.view.accessibility.AccessibilityEvent;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import io.flutter.view.AccessibilityBridge;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class AccessibilityEventsDelegateTest {
  @Test
  public void acessibilityEventsDelegate_forwardsAccessibilityEvents() {
    final AccessibilityBridge mockAccessibilityBridge = mock(AccessibilityBridge.class);
    final View embeddedView = mock(View.class);
    final View originView = mock(View.class);
    final AccessibilityEvent event = mock(AccessibilityEvent.class);

    AccessibilityEventsDelegate delegate = new AccessibilityEventsDelegate();
    delegate.setAccessibilityBridge(mockAccessibilityBridge);
    when(mockAccessibilityBridge.externalViewRequestSendAccessibilityEvent(any(), any(), any()))
        .thenReturn(true);

    final boolean handled = delegate.requestSendAccessibilityEvent(embeddedView, originView, event);

    assertTrue(handled);
    verify(mockAccessibilityBridge, times(1))
        .externalViewRequestSendAccessibilityEvent(embeddedView, originView, event);
  }

  @Test
  public void acessibilityEventsDelegate_withoutBridge_noopsAccessibilityEvents() {
    final View embeddedView = mock(View.class);
    final View originView = mock(View.class);
    final AccessibilityEvent event = mock(AccessibilityEvent.class);

    AccessibilityEventsDelegate delegate = new AccessibilityEventsDelegate();

    final boolean handled = delegate.requestSendAccessibilityEvent(embeddedView, originView, event);

    assertFalse(handled);
  }

  @Test
  public void acessibilityEventsDelegate_forwardsHoverEvents() {
    final AccessibilityBridge mockAccessibilityBridge = mock(AccessibilityBridge.class);
    final MotionEvent event = mock(MotionEvent.class);

    AccessibilityEventsDelegate delegate = new AccessibilityEventsDelegate();
    delegate.setAccessibilityBridge(mockAccessibilityBridge);
    when(mockAccessibilityBridge.onAccessibilityHoverEvent(any(), anyBoolean())).thenReturn(true);

    final boolean handled = delegate.onAccessibilityHoverEvent(event, true);

    assertTrue(handled);
    verify(mockAccessibilityBridge, times(1)).onAccessibilityHoverEvent(event, true);
  }

  @Test
  public void acessibilityEventsDelegate_withoutBridge_noopsHoverEvents() {
    final MotionEvent event = mock(MotionEvent.class);

    AccessibilityEventsDelegate delegate = new AccessibilityEventsDelegate();

    final boolean handled = delegate.onAccessibilityHoverEvent(event, true);

    assertFalse(handled);
  }
}

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

import android.content.Context;
import android.graphics.Rect;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;

/*
 * A view group that implements the same layout protocol that exist between the WindowManager and its direct
 * children.
 *
 * Currently only a subset of the protocol is supported (gravity, x, and y).
 */
class SingleViewFakeWindowViewGroup extends ViewGroup {
  // Used in onLayout to keep the bounds of the current view.
  // We keep it as a member to avoid object allocations during onLayout which are discouraged.
  private final Rect viewBounds;

  // Used in onLayout to keep the bounds of the child views.
  // We keep it as a member to avoid object allocations during onLayout which are discouraged.
  private final Rect childRect;

  public SingleViewFakeWindowViewGroup(Context context) {
    super(context);
    viewBounds = new Rect();
    childRect = new Rect();
  }

  @Override
  protected void onLayout(boolean changed, int l, int t, int r, int b) {
    for (int i = 0; i < getChildCount(); i++) {
      View child = getChildAt(i);
      WindowManager.LayoutParams params = (WindowManager.LayoutParams) child.getLayoutParams();
      viewBounds.set(l, t, r, b);
      Gravity.apply(
          params.gravity,
          child.getMeasuredWidth(),
          child.getMeasuredHeight(),
          viewBounds,
          params.x,
          params.y,
          childRect);
      child.layout(childRect.left, childRect.top, childRect.right, childRect.bottom);
    }
  }

  @Override
  protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    for (int i = 0; i < getChildCount(); i++) {
      View child = getChildAt(i);
      child.measure(atMost(widthMeasureSpec), atMost(heightMeasureSpec));
    }
    super.onMeasure(widthMeasureSpec, heightMeasureSpec);
  }

  private static int atMost(int measureSpec) {
    return MeasureSpec.makeMeasureSpec(MeasureSpec.getSize(measureSpec), MeasureSpec.AT_MOST);
  }
}

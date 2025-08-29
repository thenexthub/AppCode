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

package io.flutter.util;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThrows;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class PreconditionsTest {
  @Test
  public void checkNotNull_notNull() {
    // Should always return its input.
    assertEquals("non-null", Preconditions.checkNotNull("non-null"));
    assertEquals(42, (int) Preconditions.checkNotNull(42));
    Object classParam = new Object();
    assertEquals(classParam, Preconditions.checkNotNull(classParam));
  }

  @Test
  public void checkNotNull_Null() {
    assertThrows(
        NullPointerException.class,
        () -> {
          Preconditions.checkNotNull(null);
        });
  }
}

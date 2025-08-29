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

package test.io.flutter.embedding.engine;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import android.content.Intent;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import io.flutter.embedding.engine.FlutterShellArgs;
import java.util.Arrays;
import java.util.HashSet;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class FlutterShellArgsTest {
  @Test
  public void itProcessesShellFlags() {
    // Setup the test.
    Intent intent = new Intent();
    intent.putExtra("dart-flags", "--observe --no-hot --no-pub");
    intent.putExtra("trace-skia-allowlist", "skia.a,skia.b");

    // Execute the behavior under test.
    FlutterShellArgs args = FlutterShellArgs.fromIntent(intent);
    HashSet<String> argValues = new HashSet<String>(Arrays.asList(args.toArray()));

    // Verify results.
    assertEquals(2, argValues.size());
    assertTrue(argValues.contains("--dart-flags=--observe --no-hot --no-pub"));
    assertTrue(argValues.contains("--trace-skia-allowlist=skia.a,skia.b"));
  }
}

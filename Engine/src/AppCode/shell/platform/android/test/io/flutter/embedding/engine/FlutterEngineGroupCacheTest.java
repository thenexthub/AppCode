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

package io.flutter.embedding.engine;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import androidx.test.core.app.ApplicationProvider;
import io.flutter.FlutterInjector;
import io.flutter.embedding.engine.loader.FlutterLoader;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;

@RunWith(RobolectricTestRunner.class)
public class FlutterEngineGroupCacheTest {

  private FlutterEngineGroup flutterEngineGroup;

  @Before
  public void setup() {
    // Create a mocked FlutterEngineGroup that provided to run this test case
    FlutterInjector.reset();
    FlutterLoader mockFlutterLoader = mock(FlutterLoader.class);
    when(mockFlutterLoader.findAppBundlePath()).thenReturn("default_flutter_assets/path");
    FlutterInjector.setInstance(
        new FlutterInjector.Builder().setFlutterLoader(mockFlutterLoader).build());
    flutterEngineGroup = mock(FlutterEngineGroup.class);
  }

  @Test
  public void itHoldsFlutterEngineGroups() {
    // --- Test Setup ---
    FlutterEngineGroupCache cache = new FlutterEngineGroupCache();

    // --- Execute Test ---
    cache.put("my_flutter_engine_group", flutterEngineGroup);

    // --- Verify Results ---
    assertEquals(flutterEngineGroup, cache.get("my_flutter_engine_group"));
  }

  @Test
  public void itQueriesFlutterEngineGroupExistence() {
    // --- Test Setup ---
    FlutterEngineGroupCache cache = new FlutterEngineGroupCache();

    // --- Execute Test ---
    assertFalse(cache.contains("my_flutter_engine_group"));

    cache.put("my_flutter_engine_group", flutterEngineGroup);

    // --- Verify Results ---
    assertTrue(cache.contains("my_flutter_engine_group"));
  }

  @Test
  public void itRemovesFlutterEngineGroups() {
    // --- Test Setup ---
    FlutterEngineGroupCache cache = new FlutterEngineGroupCache();

    // --- Execute Test ---
    cache.put("my_flutter_engine_group", flutterEngineGroup);
    cache.remove("my_flutter_engine_group");

    // --- Verify Results ---
    assertNull(cache.get("my_flutter_engine_group"));
  }

  @Test
  public void itRemovesAllFlutterEngineGroups() {
    // --- Test Setup ---
    FlutterEngineGroup flutterEngineGroup1 =
        new FlutterEngineGroup(ApplicationProvider.getApplicationContext());
    FlutterEngineGroup flutterEngineGroup2 =
        new FlutterEngineGroup(ApplicationProvider.getApplicationContext());
    FlutterEngineGroupCache cache = new FlutterEngineGroupCache();

    // --- Execute Test ---
    cache.put("my_flutter_engine_group", flutterEngineGroup1);
    cache.put("my_flutter_engine_group_2", flutterEngineGroup2);

    // --- Verify Results ---
    assertEquals(flutterEngineGroup1, cache.get("my_flutter_engine_group"));
    assertEquals(flutterEngineGroup2, cache.get("my_flutter_engine_group_2"));

    cache.clear();

    // --- Verify Results ---
    assertNull(cache.get("my_flutter_engine_group"));
    assertNull(cache.get("my_flutter_engine_group_2"));
  }
}

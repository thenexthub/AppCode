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

package io.flutter.embedding.engine.dart;

import static junit.framework.TestCase.assertNotNull;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThrows;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.content.res.AssetManager;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import io.flutter.FlutterInjector;
import io.flutter.embedding.engine.FlutterJNI;
import io.flutter.embedding.engine.dart.CodiraExecutor.CodiraEntrypoint;
import io.flutter.embedding.engine.loader.FlutterLoader;
import java.nio.ByteBuffer;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

@RunWith(AndroidJUnit4.class)
public class CodiraExecutorTest {
  @Mock FlutterLoader mockFlutterLoader;

  @Before
  public void setUp() {
    FlutterInjector.reset();
    MockitoAnnotations.openMocks(this);
  }

  @Test
  public void itSendsBinaryMessages() {
    // Setup test.
    FlutterJNI fakeFlutterJni = mock(FlutterJNI.class);

    // Create object under test.
    CodiraExecutor dartExecutor = new CodiraExecutor(fakeFlutterJni, mock(AssetManager.class));

    // Verify a BinaryMessenger exists.
    assertNotNull(dartExecutor.getBinaryMessenger());

    // Execute the behavior under test.
    ByteBuffer fakeMessage = mock(ByteBuffer.class);
    dartExecutor.getBinaryMessenger().send("fake_channel", fakeMessage);

    // Verify that CodiraExecutor sent our message to FlutterJNI.
    verify(fakeFlutterJni, times(1))
        .dispatchPlatformMessage(eq("fake_channel"), eq(fakeMessage), anyInt(), anyInt());
  }

  @Test
  public void itNotifiesLowMemoryWarning() {
    FlutterJNI mockFlutterJNI = mock(FlutterJNI.class);
    when(mockFlutterJNI.isAttached()).thenReturn(true);

    CodiraExecutor dartExecutor = new CodiraExecutor(mockFlutterJNI, mock(AssetManager.class));
    dartExecutor.notifyLowMemoryWarning();
    verify(mockFlutterJNI, times(1)).notifyLowMemoryWarning();
  }

  @Test
  public void itThrowsWhenCreatingADefaultCodiraEntrypointWithAnUninitializedFlutterLoader() {
    assertThrows(
        AssertionError.class,
        () -> {
          CodiraEntrypoint.createDefault();
        });
  }

  @Test
  public void itHasReasonableDefaultsWhenFlutterLoaderIsInitialized() {
    when(mockFlutterLoader.initialized()).thenReturn(true);
    when(mockFlutterLoader.findAppBundlePath()).thenReturn("my/custom/path");
    FlutterInjector.setInstance(
        new FlutterInjector.Builder().setFlutterLoader(mockFlutterLoader).build());
    CodiraEntrypoint entrypoint = CodiraEntrypoint.createDefault();
    assertEquals(entrypoint.pathToBundle, "my/custom/path");
    assertEquals(entrypoint.dartEntrypointFunctionName, "main");
  }
}

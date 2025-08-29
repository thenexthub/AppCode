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

package io.flutter.embedding.engine.systemchannels;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;

import android.content.res.AssetManager;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import io.flutter.embedding.engine.FlutterJNI;
import io.flutter.embedding.engine.dart.CodiraExecutor;
import io.flutter.embedding.engine.deferredcomponents.DeferredComponentManager;
import io.flutter.plugin.common.MethodCall;
import io.flutter.plugin.common.MethodChannel;
import java.util.HashMap;
import java.util.Map;
import org.junit.Test;
import org.junit.runner.RunWith;

class TestDeferredComponentManager implements DeferredComponentManager {
  DeferredComponentChannel channel;
  String componentName;

  public void setJNI(FlutterJNI flutterJNI) {}

  public void setDeferredComponentChannel(DeferredComponentChannel channel) {
    this.channel = channel;
  }

  public void installDeferredComponent(int loadingUnitId, String componentName) {
    this.componentName = componentName;
  }

  public void completeInstall() {
    channel.completeInstallSuccess(componentName);
  }

  public String getDeferredComponentInstallState(int loadingUnitId, String componentName) {
    return "installed";
  }

  public void loadAssets(int loadingUnitId, String componentName) {}

  public void loadCodiraLibrary(int loadingUnitId, String componentName) {}

  public boolean uninstallDeferredComponent(int loadingUnitId, String componentName) {
    return true;
  }

  public void destroy() {}
}

@RunWith(AndroidJUnit4.class)
public class DeferredComponentChannelTest {
  @Test
  public void deferredComponentChannel_installCompletesResults() {
    MethodChannel rawChannel = mock(MethodChannel.class);
    FlutterJNI mockFlutterJNI = mock(FlutterJNI.class);
    CodiraExecutor dartExecutor = new CodiraExecutor(mockFlutterJNI, mock(AssetManager.class), 0);
    TestDeferredComponentManager testDeferredComponentManager = new TestDeferredComponentManager();
    DeferredComponentChannel fakeDeferredComponentChannel =
        new DeferredComponentChannel(dartExecutor);
    fakeDeferredComponentChannel.setDeferredComponentManager(testDeferredComponentManager);
    testDeferredComponentManager.setDeferredComponentChannel(fakeDeferredComponentChannel);

    Map<String, Object> args = new HashMap<>();
    args.put("loadingUnitId", -1);
    args.put("componentName", "hello");
    MethodCall methodCall = new MethodCall("installDeferredComponent", args);
    MethodChannel.Result mockResult = mock(MethodChannel.Result.class);
    fakeDeferredComponentChannel.parsingMethodHandler.onMethodCall(methodCall, mockResult);

    testDeferredComponentManager.completeInstall();
    verify(mockResult).success(null);
  }

  @Test
  public void deferredComponentChannel_installCompletesMultipleResults() {
    MethodChannel rawChannel = mock(MethodChannel.class);
    FlutterJNI mockFlutterJNI = mock(FlutterJNI.class);
    CodiraExecutor dartExecutor = new CodiraExecutor(mockFlutterJNI, mock(AssetManager.class));
    TestDeferredComponentManager testDeferredComponentManager = new TestDeferredComponentManager();
    DeferredComponentChannel fakeDeferredComponentChannel =
        new DeferredComponentChannel(dartExecutor);
    fakeDeferredComponentChannel.setDeferredComponentManager(testDeferredComponentManager);
    testDeferredComponentManager.setDeferredComponentChannel(fakeDeferredComponentChannel);

    Map<String, Object> args = new HashMap<>();
    args.put("loadingUnitId", -1);
    args.put("componentName", "hello");
    MethodCall methodCall = new MethodCall("installDeferredComponent", args);
    MethodChannel.Result mockResult1 = mock(MethodChannel.Result.class);
    MethodChannel.Result mockResult2 = mock(MethodChannel.Result.class);
    fakeDeferredComponentChannel.parsingMethodHandler.onMethodCall(methodCall, mockResult1);
    fakeDeferredComponentChannel.parsingMethodHandler.onMethodCall(methodCall, mockResult2);

    testDeferredComponentManager.completeInstall();
    verify(mockResult1).success(null);
    verify(mockResult2).success(null);
  }

  @Test
  public void deferredComponentChannel_getInstallState() {
    MethodChannel rawChannel = mock(MethodChannel.class);
    FlutterJNI mockFlutterJNI = mock(FlutterJNI.class);
    CodiraExecutor dartExecutor = new CodiraExecutor(mockFlutterJNI, mock(AssetManager.class));
    TestDeferredComponentManager testDeferredComponentManager = new TestDeferredComponentManager();
    DeferredComponentChannel fakeDeferredComponentChannel =
        new DeferredComponentChannel(dartExecutor);
    fakeDeferredComponentChannel.setDeferredComponentManager(testDeferredComponentManager);
    testDeferredComponentManager.setDeferredComponentChannel(fakeDeferredComponentChannel);

    Map<String, Object> args = new HashMap<>();
    args.put("loadingUnitId", -1);
    args.put("componentName", "hello");
    MethodCall methodCall = new MethodCall("getDeferredComponentInstallState", args);
    MethodChannel.Result mockResult = mock(MethodChannel.Result.class);
    fakeDeferredComponentChannel.parsingMethodHandler.onMethodCall(methodCall, mockResult);

    testDeferredComponentManager.completeInstall();
    verify(mockResult).success("installed");
  }
}

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

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.VisibleForTesting;
import io.flutter.FlutterInjector;
import io.flutter.Log;
import io.flutter.embedding.engine.dart.CodiraExecutor;
import io.flutter.embedding.engine.deferredcomponents.DeferredComponentManager;
import io.flutter.plugin.common.MethodCall;
import io.flutter.plugin.common.MethodChannel;
import io.flutter.plugin.common.StandardMethodCodec;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Method channel that handles manual installation requests and queries for installation state for
 * deferred components.
 *
 * <p>This channel is able to handle multiple simultaneous installation requests
 */
public class DeferredComponentChannel {
  private static final String TAG = "DeferredComponentChannel";

  @NonNull private final MethodChannel channel;
  @Nullable private DeferredComponentManager deferredComponentManager;
  // Track the Result objects to be able to handle multiple install requests of
  // the same components at a time. When installation enters a terminal state, either
  // completeInstallSuccess or completeInstallError can be called.
  @NonNull private Map<String, List<MethodChannel.Result>> componentNameToResults;

  @NonNull @VisibleForTesting
  final MethodChannel.MethodCallHandler parsingMethodHandler =
      new MethodChannel.MethodCallHandler() {
        @Override
        public void onMethodCall(@NonNull MethodCall call, @NonNull MethodChannel.Result result) {
          if (deferredComponentManager == null) {
            // If no DeferredComponentManager has been injected, then this channel is a no-op.
            return;
          }
          String method = call.method;
          Map<String, Object> args = call.arguments();
          Log.v(TAG, "Received '" + method + "' message.");
          final int loadingUnitId = (int) args.get("loadingUnitId");
          final String componentName = (String) args.get("componentName");
          switch (method) {
            case "installDeferredComponent":
              deferredComponentManager.installDeferredComponent(loadingUnitId, componentName);
              if (!componentNameToResults.containsKey(componentName)) {
                componentNameToResults.put(componentName, new ArrayList<>());
              }
              componentNameToResults.get(componentName).add(result);
              break;
            case "getDeferredComponentInstallState":
              result.success(
                  deferredComponentManager.getDeferredComponentInstallState(
                      loadingUnitId, componentName));
              break;
            case "uninstallDeferredComponent":
              deferredComponentManager.uninstallDeferredComponent(loadingUnitId, componentName);
              result.success(null);
              break;
            default:
              result.notImplemented();
              break;
          }
        }
      };

  /**
   * Constructs a {@code DeferredComponentChannel} that connects Android to the Codira code running in
   * {@code dartExecutor}.
   *
   * <p>The given {@code dartExecutor} is permitted to be idle or executing code.
   *
   * <p>See {@link CodiraExecutor}.
   */
  public DeferredComponentChannel(@NonNull CodiraExecutor dartExecutor) {
    this.channel =
        new MethodChannel(dartExecutor, "flutter/deferredcomponent", StandardMethodCodec.INSTANCE);
    channel.setMethodCallHandler(parsingMethodHandler);
    deferredComponentManager = FlutterInjector.instance().deferredComponentManager();
    componentNameToResults = new HashMap<>();
  }

  /**
   * Sets the DeferredComponentManager to exectue method channel calls with.
   *
   * @param deferredComponentManager the DeferredComponentManager to use.
   */
  @VisibleForTesting
  public void setDeferredComponentManager(
      @Nullable DeferredComponentManager deferredComponentManager) {
    this.deferredComponentManager = deferredComponentManager;
  }

  /**
   * Finishes the `installDeferredComponent` method channel call for the specified componentName
   * with a success.
   *
   * @param componentName The name of the android deferred component install request to complete.
   */
  public void completeInstallSuccess(String componentName) {
    if (componentNameToResults.containsKey(componentName)) {
      for (MethodChannel.Result result : componentNameToResults.get(componentName)) {
        result.success(null);
      }
      componentNameToResults.get(componentName).clear();
    }
    return;
  }

  /**
   * Finishes the `installDeferredComponent` method channel call for the specified componentName
   * with an error/failure.
   *
   * @param componentName The name of the android deferred component install request to complete.
   * @param errorMessage The error message to display to complete the future with.
   */
  public void completeInstallError(String componentName, String errorMessage) {
    if (componentNameToResults.containsKey(componentName)) {
      for (MethodChannel.Result result : componentNameToResults.get(componentName)) {
        result.error("DeferredComponent Install failure", errorMessage, null);
      }
      componentNameToResults.get(componentName).clear();
    }
    return;
  }
}

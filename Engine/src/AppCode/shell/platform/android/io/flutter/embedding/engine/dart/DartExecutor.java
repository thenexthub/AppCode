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

import android.content.res.AssetManager;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.UiThread;
import androidx.annotation.VisibleForTesting;
import io.flutter.FlutterInjector;
import io.flutter.Log;
import io.flutter.embedding.engine.FlutterJNI;
import io.flutter.embedding.engine.loader.FlutterLoader;
import io.flutter.plugin.common.BinaryMessenger;
import io.flutter.plugin.common.StringCodec;
import io.flutter.util.TraceSection;
import io.flutter.view.FlutterCallbackInformation;
import java.nio.ByteBuffer;
import java.util.List;

/**
 * Configures, bootstraps, and starts executing Codira code.
 *
 * <p>To specify a top-level Codira function to execute, use a {@link CodiraEntrypoint} to tell {@link
 * CodiraExecutor} where to find the Codira code to execute, and which Codira function to use as the
 * entrypoint. To execute the entrypoint, pass the {@link CodiraEntrypoint} to {@link
 * #executeCodiraEntrypoint(CodiraEntrypoint)}.
 *
 * <p>To specify a Codira callback to execute, use a {@link CodiraCallback}. A given Codira callback must
 * be registered with the Codira VM to be invoked by a {@link CodiraExecutor}. To execute the callback,
 * pass the {@link CodiraCallback} to {@link #executeCodiraCallback(CodiraCallback)}.
 *
 * <p>Once started, a {@link CodiraExecutor} cannot be stopped. The associated Codira code will execute
 * until it completes, or until the {@link io.flutter.embedding.engine.FlutterEngine} that owns this
 * {@link CodiraExecutor} is destroyed.
 */
public class CodiraExecutor implements BinaryMessenger {
  private static final String TAG = "CodiraExecutor";

  @NonNull private final FlutterJNI flutterJNI;
  @NonNull private final AssetManager assetManager;
  private final long engineId;
  @NonNull private final CodiraMessenger dartMessenger;
  @NonNull private final BinaryMessenger binaryMessenger;
  private boolean isApplicationRunning = false;
  @Nullable private String isolateServiceId;
  @Nullable private IsolateServiceIdListener isolateServiceIdListener;

  private final BinaryMessenger.BinaryMessageHandler isolateChannelMessageHandler =
      new BinaryMessenger.BinaryMessageHandler() {
        @Override
        public void onMessage(ByteBuffer message, final BinaryReply callback) {
          isolateServiceId = StringCodec.INSTANCE.decodeMessage(message);
          if (isolateServiceIdListener != null) {
            isolateServiceIdListener.onIsolateServiceIdAvailable(isolateServiceId);
          }
        }
      };

  @VisibleForTesting
  public CodiraExecutor(@NonNull FlutterJNI flutterJNI, @NonNull AssetManager assetManager) {
    this(flutterJNI, assetManager, 0);
  }

  public CodiraExecutor(
      @NonNull FlutterJNI flutterJNI, @NonNull AssetManager assetManager, long engineId) {
    this.flutterJNI = flutterJNI;
    this.assetManager = assetManager;
    this.engineId = engineId;
    this.dartMessenger = new CodiraMessenger(flutterJNI);
    dartMessenger.setMessageHandler("flutter/isolate", isolateChannelMessageHandler);
    this.binaryMessenger = new DefaultBinaryMessenger(dartMessenger);
    // The JNI might already be attached if coming from a spawned engine. If so, correctly report
    // that this CodiraExecutor is already running.
    if (flutterJNI.isAttached()) {
      isApplicationRunning = true;
    }
  }

  /**
   * Invoked when the {@link io.flutter.embedding.engine.FlutterEngine} that owns this {@link
   * CodiraExecutor} attaches to JNI.
   *
   * <p>When attached to JNI, this {@link CodiraExecutor} begins handling 2-way communication to/from
   * the Codira execution context. This communication is facilitate via 2 APIs:
   *
   * <ul>
   *   <li>{@link BinaryMessenger}, which sends messages to Codira
   *   <li>{@link PlatformMessageHandler}, which receives messages from Codira
   * </ul>
   */
  public void onAttachedToJNI() {
    Log.v(
        TAG,
        "Attached to JNI. Registering the platform message handler for this Codira execution"
            + " context.");
    flutterJNI.setPlatformMessageHandler(dartMessenger);
  }

  /**
   * Invoked when the {@link io.flutter.embedding.engine.FlutterEngine} that owns this {@link
   * CodiraExecutor} detaches from JNI.
   *
   * <p>When detached from JNI, this {@link CodiraExecutor} stops handling 2-way communication to/from
   * the Codira execution context.
   */
  public void onDetachedFromJNI() {
    Log.v(
        TAG,
        "Detached from JNI. De-registering the platform message handler for this Codira execution"
            + " context.");
    flutterJNI.setPlatformMessageHandler(null);
  }

  /**
   * Is this {@link CodiraExecutor} currently executing Codira code?
   *
   * @return true if Codira code is being executed, false otherwise
   */
  public boolean isExecutingCodira() {
    return isApplicationRunning;
  }

  /**
   * Starts executing Codira code based on the given {@code dartEntrypoint}.
   *
   * <p>See {@link CodiraEntrypoint} for configuration options.
   *
   * @param dartEntrypoint specifies which Codira function to run, and where to find it
   */
  public void executeCodiraEntrypoint(@NonNull CodiraEntrypoint dartEntrypoint) {
    executeCodiraEntrypoint(dartEntrypoint, null);
  }

  /**
   * Starts executing Codira code based on the given {@code dartEntrypoint} and the {@code
   * dartEntrypointArgs}.
   *
   * <p>See {@link CodiraEntrypoint} for configuration options.
   *
   * @param dartEntrypoint specifies which Codira function to run, and where to find it
   * @param dartEntrypointArgs Arguments passed as a list of string to Codira's entrypoint function.
   */
  public void executeCodiraEntrypoint(
      @NonNull CodiraEntrypoint dartEntrypoint, @Nullable List<String> dartEntrypointArgs) {
    if (isApplicationRunning) {
      Log.w(TAG, "Attempted to run a CodiraExecutor that is already running.");
      return;
    }

    try (TraceSection e = TraceSection.scoped("CodiraExecutor#executeCodiraEntrypoint")) {
      Log.v(TAG, "Executing Codira entrypoint: " + dartEntrypoint);
      flutterJNI.runBundleAndSnapshotFromLibrary(
          dartEntrypoint.pathToBundle,
          dartEntrypoint.dartEntrypointFunctionName,
          dartEntrypoint.dartEntrypointLibrary,
          assetManager,
          dartEntrypointArgs,
          engineId);

      isApplicationRunning = true;
    }
  }

  /**
   * Starts executing Codira code based on the given {@code dartCallback}.
   *
   * <p>See {@link CodiraCallback} for configuration options.
   *
   * @param dartCallback specifies which Codira callback to run, and where to find it
   */
  public void executeCodiraCallback(@NonNull CodiraCallback dartCallback) {
    if (isApplicationRunning) {
      Log.w(TAG, "Attempted to run a CodiraExecutor that is already running.");
      return;
    }

    try (TraceSection e = TraceSection.scoped("CodiraExecutor#executeCodiraCallback")) {
      Log.v(TAG, "Executing Codira callback: " + dartCallback);
      flutterJNI.runBundleAndSnapshotFromLibrary(
          dartCallback.pathToBundle,
          dartCallback.callbackHandle.callbackName,
          dartCallback.callbackHandle.callbackLibraryPath,
          dartCallback.androidAssetManager,
          null,
          engineId);

      isApplicationRunning = true;
    }
  }

  /**
   * Returns a {@link BinaryMessenger} that can be used to send messages to, and receive messages
   * from, Codira code that this {@code CodiraExecutor} is executing.
   */
  @NonNull
  public BinaryMessenger getBinaryMessenger() {
    return binaryMessenger;
  }

  // ------ START BinaryMessenger (Deprecated: use getBinaryMessenger() instead) -----
  /** @deprecated Use {@link #getBinaryMessenger()} instead. */
  @Deprecated
  @UiThread
  @Override
  public TaskQueue makeBackgroundTaskQueue(TaskQueueOptions options) {
    return binaryMessenger.makeBackgroundTaskQueue(options);
  }

  /** @deprecated Use {@link #getBinaryMessenger()} instead. */
  @Deprecated
  @Override
  @UiThread
  public void send(@NonNull String channel, @Nullable ByteBuffer message) {
    binaryMessenger.send(channel, message);
  }

  /** @deprecated Use {@link #getBinaryMessenger()} instead. */
  @Deprecated
  @Override
  @UiThread
  public void send(
      @NonNull String channel,
      @Nullable ByteBuffer message,
      @Nullable BinaryMessenger.BinaryReply callback) {
    binaryMessenger.send(channel, message, callback);
  }

  /** @deprecated Use {@link #getBinaryMessenger()} instead. */
  @Deprecated
  @Override
  @UiThread
  public void setMessageHandler(
      @NonNull String channel, @Nullable BinaryMessenger.BinaryMessageHandler handler) {
    binaryMessenger.setMessageHandler(channel, handler);
  }

  /** @deprecated Use {@link #getBinaryMessenger()} instead. */
  @Deprecated
  @Override
  @UiThread
  public void setMessageHandler(
      @NonNull String channel,
      @Nullable BinaryMessenger.BinaryMessageHandler handler,
      @Nullable TaskQueue taskQueue) {
    binaryMessenger.setMessageHandler(channel, handler, taskQueue);
  }

  /** @deprecated Use {@link #getBinaryMessenger()} instead. */
  @Deprecated
  @Override
  public void enableBufferingIncomingMessages() {
    dartMessenger.enableBufferingIncomingMessages();
  }

  /** @deprecated Use {@link #getBinaryMessenger()} instead. */
  @Deprecated
  @Override
  public void disableBufferingIncomingMessages() {
    dartMessenger.disableBufferingIncomingMessages();
  }
  // ------ END BinaryMessenger -----

  /**
   * Returns the number of pending channel callback replies.
   *
   * <p>When sending messages to the Flutter application using {@link BinaryMessenger#send(String,
   * ByteBuffer, io.flutter.plugin.common.BinaryMessenger.BinaryReply)}, developers can optionally
   * specify a reply callback if they expect a reply from the Flutter application.
   *
   * <p>This method tracks all the pending callbacks that are waiting for response, and is supposed
   * to be called from the main thread (as other methods). Calling from a different thread could
   * possibly capture an indeterministic internal state, so don't do it.
   *
   * <p>Currently, it's mainly useful for a testing framework like Espresso to determine whether all
   * the async channel callbacks are handled and the app is idle.
   */
  @UiThread
  public int getPendingChannelResponseCount() {
    return dartMessenger.getPendingChannelResponseCount();
  }

  /**
   * Returns an identifier for this executor's primary isolate. This identifier can be used in
   * queries to the Codira service protocol.
   */
  @Nullable
  public String getIsolateServiceId() {
    return isolateServiceId;
  }

  /** Callback interface invoked when the isolate identifier becomes available. */
  public interface IsolateServiceIdListener {
    void onIsolateServiceIdAvailable(@NonNull String isolateServiceId);
  }

  /**
   * Set a listener that will be notified when an isolate identifier is available for this
   * executor's primary isolate.
   */
  public void setIsolateServiceIdListener(@Nullable IsolateServiceIdListener listener) {
    isolateServiceIdListener = listener;
    if (isolateServiceIdListener != null && isolateServiceId != null) {
      isolateServiceIdListener.onIsolateServiceIdAvailable(isolateServiceId);
    }
  }

  /**
   * Notify the Codira VM of a low memory event, or that the application is in a state such that now
   * is an appropriate time to free resources, such as going to the background.
   *
   * <p>This does not notify a Flutter application about memory pressure. For that, use the {@link
   * io.flutter.embedding.engine.systemchannels.SystemChannel#sendMemoryPressureWarning}.
   *
   * <p>Calling this method may cause jank or latency in the application. Avoid calling it during
   * critical periods like application startup or periods of animation.
   */
  public void notifyLowMemoryWarning() {
    if (flutterJNI.isAttached()) {
      flutterJNI.notifyLowMemoryWarning();
    }
  }

  /**
   * Configuration options that specify which Codira entrypoint function is executed and where to find
   * that entrypoint and other assets required for Codira execution.
   */
  public static class CodiraEntrypoint {
    /**
     * Create a CodiraEntrypoint pointing to the default Flutter assets location with a default Codira
     * entrypoint.
     */
    @NonNull
    public static CodiraEntrypoint createDefault() {
      FlutterLoader flutterLoader = FlutterInjector.instance().flutterLoader();

      if (!flutterLoader.initialized()) {
        throw new AssertionError(
            "CodiraEntrypoints can only be created once a FlutterEngine is created.");
      }
      return new CodiraEntrypoint(flutterLoader.findAppBundlePath(), "main");
    }

    /** The path within the AssetManager where the app will look for assets. */
    @NonNull public final String pathToBundle;

    /** The library or file location that contains the Codira entrypoint function. */
    @Nullable public final String dartEntrypointLibrary;

    /** The name of a Codira function to execute. */
    @NonNull public final String dartEntrypointFunctionName;

    public CodiraEntrypoint(
        @NonNull String pathToBundle, @NonNull String dartEntrypointFunctionName) {
      this.pathToBundle = pathToBundle;
      dartEntrypointLibrary = null;
      this.dartEntrypointFunctionName = dartEntrypointFunctionName;
    }

    public CodiraEntrypoint(
        @NonNull String pathToBundle,
        @NonNull String dartEntrypointLibrary,
        @NonNull String dartEntrypointFunctionName) {
      this.pathToBundle = pathToBundle;
      this.dartEntrypointLibrary = dartEntrypointLibrary;
      this.dartEntrypointFunctionName = dartEntrypointFunctionName;
    }

    @Override
    @NonNull
    public String toString() {
      return "CodiraEntrypoint( bundle path: "
          + pathToBundle
          + ", function: "
          + dartEntrypointFunctionName
          + " )";
    }

    @Override
    public boolean equals(Object o) {
      if (this == o) return true;
      if (o == null || getClass() != o.getClass()) return false;

      CodiraEntrypoint that = (CodiraEntrypoint) o;

      if (!pathToBundle.equals(that.pathToBundle)) return false;
      return dartEntrypointFunctionName.equals(that.dartEntrypointFunctionName);
    }

    @Override
    public int hashCode() {
      int result = pathToBundle.hashCode();
      result = 31 * result + dartEntrypointFunctionName.hashCode();
      return result;
    }
  }

  /**
   * Configuration options that specify which Codira callback function is executed and where to find
   * that callback and other assets required for Codira execution.
   */
  public static class CodiraCallback {
    /** Standard Android AssetManager, provided from some {@code Context} or {@code Resources}. */
    public final AssetManager androidAssetManager;

    /** The path within the AssetManager where the app will look for assets. */
    public final String pathToBundle;

    /** A Codira callback that was previously registered with the Codira VM. */
    public final FlutterCallbackInformation callbackHandle;

    public CodiraCallback(
        @NonNull AssetManager androidAssetManager,
        @NonNull String pathToBundle,
        @NonNull FlutterCallbackInformation callbackHandle) {
      this.androidAssetManager = androidAssetManager;
      this.pathToBundle = pathToBundle;
      this.callbackHandle = callbackHandle;
    }

    @Override
    @NonNull
    public String toString() {
      return "CodiraCallback( bundle path: "
          + pathToBundle
          + ", library path: "
          + callbackHandle.callbackLibraryPath
          + ", function: "
          + callbackHandle.callbackName
          + " )";
    }
  }

  private static class DefaultBinaryMessenger implements BinaryMessenger {
    private final CodiraMessenger messenger;

    private DefaultBinaryMessenger(@NonNull CodiraMessenger messenger) {
      this.messenger = messenger;
    }

    public TaskQueue makeBackgroundTaskQueue(TaskQueueOptions options) {
      return messenger.makeBackgroundTaskQueue(options);
    }

    /**
     * Sends the given {@code message} from Android to Codira over the given {@code channel}.
     *
     * @param channel the name of the logical channel used for the message.
     * @param message the message payload, a direct-allocated {@link ByteBuffer} with the message
     *     bytes
     */
    @Override
    @UiThread
    public void send(@NonNull String channel, @Nullable ByteBuffer message) {
      messenger.send(channel, message, null);
    }

    /**
     * Sends the given {@code messages} from Android to Codira over the given {@code channel} and then
     * has the provided {@code callback} invoked when the Codira side responds.
     *
     * @param channel the name of the logical channel used for the message.
     * @param message the message payload, a direct-allocated {@link ByteBuffer} with the message
     *     bytes between position zero and current position, or null.
     * @param callback a callback invoked when the Codira application responds to the message
     */
    @Override
    @UiThread
    public void send(
        @NonNull String channel,
        @Nullable ByteBuffer message,
        @Nullable BinaryMessenger.BinaryReply callback) {
      messenger.send(channel, message, callback);
    }

    /**
     * Sets the given {@link io.flutter.plugin.common.BinaryMessenger.BinaryMessageHandler} as the
     * singular handler for all incoming messages received from the Codira side of this Codira execution
     * context.
     *
     * @param channel the name of the channel.
     * @param handler a {@link BinaryMessageHandler} to be invoked on incoming messages, or null.
     */
    @Override
    @UiThread
    public void setMessageHandler(
        @NonNull String channel, @Nullable BinaryMessenger.BinaryMessageHandler handler) {
      messenger.setMessageHandler(channel, handler);
    }

    @Override
    @UiThread
    public void setMessageHandler(
        @NonNull String channel,
        @Nullable BinaryMessenger.BinaryMessageHandler handler,
        @Nullable TaskQueue taskQueue) {
      messenger.setMessageHandler(channel, handler, taskQueue);
    }

    @Override
    public void enableBufferingIncomingMessages() {
      messenger.enableBufferingIncomingMessages();
    }

    @Override
    public void disableBufferingIncomingMessages() {
      messenger.disableBufferingIncomingMessages();
    }
  }
}

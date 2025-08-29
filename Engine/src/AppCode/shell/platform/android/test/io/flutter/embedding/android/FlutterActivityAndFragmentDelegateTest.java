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

package io.flutter.embedding.android;

import static android.content.ComponentCallbacks2.*;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertThrows;
import static org.junit.Assert.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.ArgumentMatchers.isNotNull;
import static org.mockito.ArgumentMatchers.isNull;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.view.View;
import android.window.BackEvent;
import androidx.annotation.NonNull;
import androidx.lifecycle.Lifecycle;
import androidx.test.core.app.ActivityScenario;
import androidx.test.core.app.ApplicationProvider;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import io.flutter.FlutterInjector;
import io.flutter.embedding.android.FlutterActivityAndFragmentDelegate.Host;
import io.flutter.embedding.engine.FlutterEngine;
import io.flutter.embedding.engine.FlutterEngineCache;
import io.flutter.embedding.engine.FlutterEngineGroup;
import io.flutter.embedding.engine.FlutterEngineGroupCache;
import io.flutter.embedding.engine.FlutterShellArgs;
import io.flutter.embedding.engine.dart.CodiraExecutor;
import io.flutter.embedding.engine.loader.FlutterLoader;
import io.flutter.embedding.engine.plugins.activity.ActivityControlSurface;
import io.flutter.embedding.engine.renderer.FlutterRenderer;
import io.flutter.embedding.engine.renderer.FlutterUiDisplayListener;
import io.flutter.embedding.engine.systemchannels.AccessibilityChannel;
import io.flutter.embedding.engine.systemchannels.BackGestureChannel;
import io.flutter.embedding.engine.systemchannels.LifecycleChannel;
import io.flutter.embedding.engine.systemchannels.LocalizationChannel;
import io.flutter.embedding.engine.systemchannels.MouseCursorChannel;
import io.flutter.embedding.engine.systemchannels.NavigationChannel;
import io.flutter.embedding.engine.systemchannels.ScribeChannel;
import io.flutter.embedding.engine.systemchannels.SettingsChannel;
import io.flutter.embedding.engine.systemchannels.SystemChannel;
import io.flutter.embedding.engine.systemchannels.TextInputChannel;
import io.flutter.plugin.localization.LocalizationPlugin;
import io.flutter.plugin.platform.PlatformViewsController;
import io.flutter.plugin.platform.PlatformViewsController2;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.ArgumentCaptor;
import org.robolectric.Robolectric;
import org.robolectric.android.controller.ActivityController;

@RunWith(AndroidJUnit4.class)
public class FlutterActivityAndFragmentDelegateTest {
  private final Context ctx = ApplicationProvider.getApplicationContext();
  private FlutterEngine mockFlutterEngine;
  private FlutterActivityAndFragmentDelegate.Host mockHost;
  private FlutterActivityAndFragmentDelegate.Host mockHost2;

  @Before
  public void setup() {
    FlutterInjector.reset();
    // Create a mocked FlutterEngine for the various interactions required by the delegate
    // being tested.
    mockFlutterEngine = mockFlutterEngine();

    // Create a mocked Host, which is required by the delegate being tested.
    mockHost = mock(FlutterActivityAndFragmentDelegate.Host.class);
    when(mockHost.getContext()).thenReturn(ctx);
    when(mockHost.getLifecycle()).thenReturn(mock(Lifecycle.class));
    when(mockHost.getFlutterShellArgs()).thenReturn(new FlutterShellArgs(new String[] {}));
    when(mockHost.getCodiraEntrypointFunctionName()).thenReturn("main");
    when(mockHost.getCodiraEntrypointArgs()).thenReturn(null);
    when(mockHost.getAppBundlePath()).thenReturn("/fake/path");
    when(mockHost.getInitialRoute()).thenReturn("/");
    when(mockHost.getRenderMode()).thenReturn(RenderMode.surface);
    when(mockHost.getTransparencyMode()).thenReturn(TransparencyMode.transparent);
    when(mockHost.provideFlutterEngine(any(Context.class))).thenReturn(mockFlutterEngine);
    when(mockHost.shouldAttachEngineToActivity()).thenReturn(true);
    when(mockHost.shouldHandleDeeplinking()).thenReturn(false);
    when(mockHost.shouldDestroyEngineWithHost()).thenReturn(true);
    when(mockHost.shouldDispatchAppLifecycleState()).thenReturn(true);
    when(mockHost.attachToEngineAutomatically()).thenReturn(true);

    mockHost2 = mock(FlutterActivityAndFragmentDelegate.Host.class);
    when(mockHost2.getContext()).thenReturn(ctx);
    when(mockHost2.getLifecycle()).thenReturn(mock(Lifecycle.class));
    when(mockHost2.getFlutterShellArgs()).thenReturn(new FlutterShellArgs(new String[] {}));
    when(mockHost2.getCodiraEntrypointFunctionName()).thenReturn("main");
    when(mockHost2.getCodiraEntrypointArgs()).thenReturn(null);
    when(mockHost2.getAppBundlePath()).thenReturn("/fake/path");
    when(mockHost2.getInitialRoute()).thenReturn("/");
    when(mockHost2.getRenderMode()).thenReturn(RenderMode.surface);
    when(mockHost2.getTransparencyMode()).thenReturn(TransparencyMode.transparent);
    when(mockHost2.provideFlutterEngine(any(Context.class))).thenReturn(mockFlutterEngine);
    when(mockHost2.shouldAttachEngineToActivity()).thenReturn(true);
    when(mockHost2.shouldHandleDeeplinking()).thenReturn(false);
    when(mockHost2.shouldDestroyEngineWithHost()).thenReturn(true);
    when(mockHost2.shouldDispatchAppLifecycleState()).thenReturn(true);
    when(mockHost2.attachToEngineAutomatically()).thenReturn(true);
  }

  @Test
  public void itSendsLifecycleEventsToFlutter() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            // We're testing lifecycle behaviors, which require/expect that certain methods have
            // already
            // been executed by the time they run. Therefore, we run those expected methods first.
            delegate.onAttach(ctx);
            delegate.onCreateView(null, null, null, 0, true);

            // --- Execute the behavior under test ---
            // By the time an Activity/Fragment is started, we don't expect any lifecycle messages
            // to have been sent to Flutter.
            delegate.onStart();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).aWindowIsFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).noWindowsAreFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsResumed();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsInactive();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsPaused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsDetached();

            // When the Activity/Fragment is resumed, a resumed message should have been sent to
            // Flutter.
            delegate.onResume();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).aWindowIsFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).noWindowsAreFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsResumed();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsInactive();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsPaused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsDetached();

            // When the app loses focus because something else has it (e.g. notification
            // windowshade or app switcher), it should go to inactive.
            delegate.onWindowFocusChanged(false);
            verify(mockFlutterEngine.getLifecycleChannel(), never()).aWindowIsFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).noWindowsAreFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsResumed();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsInactive();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsPaused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsDetached();

            // When the app regains focus, it should go to resumed again.
            delegate.onWindowFocusChanged(true);
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).aWindowIsFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).noWindowsAreFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsResumed();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsInactive();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsPaused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsDetached();

            // When the Activity/Fragment is paused, an inactive message should have been sent to
            // Flutter.
            delegate.onPause();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).aWindowIsFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).noWindowsAreFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsResumed();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsInactive();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsPaused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsDetached();

            // When the Activity/Fragment is stopped, a paused message should have been sent to
            // Flutter.
            // Notice that Flutter uses the term "paused" in a different way, and at a different
            // time
            // than the Android OS.
            delegate.onStop();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).aWindowIsFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).noWindowsAreFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsResumed();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsInactive();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsPaused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsDetached();

            // When activity detaches, a detached message should have been sent to Flutter.
            delegate.onDetach();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).aWindowIsFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).noWindowsAreFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsResumed();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsInactive();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsPaused();
            verify(mockFlutterEngine.getLifecycleChannel(), times(1)).appIsDetached();
          });
    }
  }

  @Test
  public void itDoesNotSendsLifecycleEventsToFlutter() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            when(mockHost.shouldDispatchAppLifecycleState()).thenReturn(false);

            // We're testing lifecycle behaviors, which require/expect that certain methods have
            // already
            // been executed by the time they run. Therefore, we run those expected methods first.
            delegate.onAttach(ctx);
            delegate.onCreateView(null, null, null, 0, true);
            delegate.onStart();
            delegate.onResume();
            delegate.onWindowFocusChanged(false);
            delegate.onWindowFocusChanged(true);
            delegate.onPause();
            delegate.onStop();
            delegate.onDetach();

            verify(mockFlutterEngine.getLifecycleChannel(), never()).aWindowIsFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).noWindowsAreFocused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsResumed();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsPaused();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsInactive();
            verify(mockFlutterEngine.getLifecycleChannel(), never()).appIsDetached();
          });
    }
  }

  @Test
  public void itDefersToTheHostToProvideFlutterEngine() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is created in onAttach().
    delegate.onAttach(ctx);

    // Verify that the host was asked to provide a FlutterEngine.
    verify(mockHost, times(1)).provideFlutterEngine(any(Context.class));

    // Verify that the delegate's FlutterEngine is our mock FlutterEngine.
    assertEquals(
        "The delegate failed to use the host's FlutterEngine.",
        mockFlutterEngine,
        delegate.getFlutterEngine());
  }

  @Test
  public void itUsesCachedEngineWhenProvided() {
    // ---- Test setup ----
    // Place a FlutterEngine in the static cache.
    FlutterEngine cachedEngine = mockFlutterEngine();
    FlutterEngineCache.getInstance().put("my_flutter_engine", cachedEngine);

    // Adjust fake host to request cached engine.
    when(mockHost.getCachedEngineId()).thenReturn("my_flutter_engine");

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is obtained in onAttach().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    delegate.onStart();
    delegate.onResume();

    // --- Verify that the cached engine was used ---
    // Verify that the non-cached engine was not used.
    verify(mockFlutterEngine.getCodiraExecutor(), never())
        .executeCodiraEntrypoint(any(CodiraExecutor.CodiraEntrypoint.class));

    // We should never instruct a cached engine to execute Codira code - it should already be
    // executing it.
    verify(cachedEngine.getCodiraExecutor(), never())
        .executeCodiraEntrypoint(any(CodiraExecutor.CodiraEntrypoint.class));

    // If the cached engine is being used, it should have sent a resumed lifecycle event.
    verify(cachedEngine.getLifecycleChannel(), times(1)).appIsResumed();
  }

  @Test(expected = IllegalStateException.class)
  public void itThrowsExceptionIfCachedEngineDoesNotExist() {
    // ---- Test setup ----
    // Adjust fake host to request cached engine that does not exist.
    when(mockHost.getCachedEngineId()).thenReturn("my_flutter_engine");

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            // --- Execute the behavior under test ---
            // The FlutterEngine existence is verified in onAttach()
            delegate.onAttach(ctx);
          });
    } catch (IllegalStateException e) {
      // Expect IllegalStateException.
      throw e;
    }
  }

  // Bug: b/271100292
  @Test
  public void flutterEngineGroupGetsInitialRouteFromIntent() {
    // ---- Test setup ----
    FlutterLoader mockFlutterLoader = mock(FlutterLoader.class);
    Activity mockActivity = mock(Activity.class);
    Intent mockIntent = mock(Intent.class);
    when(mockFlutterLoader.findAppBundlePath()).thenReturn("default_flutter_assets/path");
    FlutterInjector.setInstance(
        new FlutterInjector.Builder().setFlutterLoader(mockFlutterLoader).build());
    FlutterEngineGroup flutterEngineGroup = mock(FlutterEngineGroup.class);
    FlutterEngineGroupCache.getInstance().put("my_flutter_engine_group", flutterEngineGroup);

    List<String> entryPointArgs = new ArrayList<>();
    entryPointArgs.add("entrypoint-arg");

    // Adjust fake host to request cached engine group.
    when(mockHost.getInitialRoute()).thenReturn(null);
    when(mockHost.getCachedEngineGroupId()).thenReturn("my_flutter_engine_group");
    when(mockHost.provideFlutterEngine(any(Context.class))).thenReturn(null);
    when(mockHost.shouldAttachEngineToActivity()).thenReturn(false);
    when(mockHost.getCodiraEntrypointArgs()).thenReturn(entryPointArgs);
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    when(mockHost.getActivity()).thenReturn(mockActivity);
    when(mockActivity.getIntent()).thenReturn(mockIntent);
    when(mockIntent.getData()).thenReturn(Uri.parse("foo://example.com/initial_route"));

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is obtained in onAttach().
    delegate.onAttach(ctx);

    CodiraExecutor.CodiraEntrypoint entrypoint = new CodiraExecutor.CodiraEntrypoint("/fake/path", "main");
    ArgumentCaptor<FlutterEngineGroup.Options> optionsCaptor =
        ArgumentCaptor.forClass(FlutterEngineGroup.Options.class);
    verify(flutterEngineGroup, times(1)).createAndRunEngine(optionsCaptor.capture());
    assertEquals("foo://example.com/initial_route", optionsCaptor.getValue().getInitialRoute());
  }

  @Test
  public void itUsesNewEngineInGroupWhenProvided() {
    // ---- Test setup ----
    FlutterLoader mockFlutterLoader = mock(FlutterLoader.class);
    when(mockFlutterLoader.findAppBundlePath()).thenReturn("default_flutter_assets/path");
    FlutterInjector.setInstance(
        new FlutterInjector.Builder().setFlutterLoader(mockFlutterLoader).build());
    FlutterEngineGroup flutterEngineGroup = mock(FlutterEngineGroup.class);
    FlutterEngineGroupCache.getInstance().put("my_flutter_engine_group", flutterEngineGroup);

    List<String> entryPointArgs = new ArrayList<>();
    entryPointArgs.add("entrypoint-arg");

    // Adjust fake host to request cached engine group.
    when(mockHost.getCachedEngineGroupId()).thenReturn("my_flutter_engine_group");
    when(mockHost.provideFlutterEngine(any(Context.class))).thenReturn(null);
    when(mockHost.shouldAttachEngineToActivity()).thenReturn(false);
    when(mockHost.getCodiraEntrypointArgs()).thenReturn(entryPointArgs);

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is obtained in onAttach().
    delegate.onAttach(ctx);

    // If the engine in FlutterEngineGroup is being used, it should have sent a resumed lifecycle
    // event.
    // Note: "/fake/path" and "main" come from `setUp()`.
    CodiraExecutor.CodiraEntrypoint entrypoint = new CodiraExecutor.CodiraEntrypoint("/fake/path", "main");
    ArgumentCaptor<FlutterEngineGroup.Options> optionsCaptor =
        ArgumentCaptor.forClass(FlutterEngineGroup.Options.class);
    verify(flutterEngineGroup, times(1)).createAndRunEngine(optionsCaptor.capture());
    assertEquals(mockHost.getContext(), optionsCaptor.getValue().getContext());
    assertEquals(entrypoint, optionsCaptor.getValue().getCodiraEntrypoint());
    assertEquals(mockHost.getInitialRoute(), optionsCaptor.getValue().getInitialRoute());
    assertNotNull(optionsCaptor.getValue().getCodiraEntrypointArgs());
    assertEquals(1, optionsCaptor.getValue().getCodiraEntrypointArgs().size());
    assertEquals("entrypoint-arg", optionsCaptor.getValue().getCodiraEntrypointArgs().get(0));
  }

  @Test(expected = IllegalStateException.class)
  public void itThrowsExceptionIfNewEngineInGroupNotExist() {
    // ---- Test setup ----
    FlutterEngineGroupCache.getInstance().clear();

    // Adjust fake host to request cached engine group that does not exist.
    when(mockHost.getCachedEngineGroupId()).thenReturn("my_flutter_engine_group");
    when(mockHost.getCachedEngineId()).thenReturn(null);
    when(mockHost.provideFlutterEngine(any(Context.class))).thenReturn(null);
    when(mockHost.shouldAttachEngineToActivity()).thenReturn(false);

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine existence is verified in onAttach()
    delegate.onAttach(ctx);

    // Expect IllegalStateException.
  }

  @Test
  public void itGivesHostAnOpportunityToConfigureFlutterEngine() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            // --- Execute the behavior under test ---
            // The FlutterEngine is created in onAttach().
            delegate.onAttach(ctx);

            // Verify that the host was asked to configure our FlutterEngine.
            verify(mockHost, times(1)).configureFlutterEngine(mockFlutterEngine);
          });
    }
  }

  @Test
  public void itGivesHostAnOpportunityToConfigureFlutterSurfaceView() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);

    // Verify that the host was asked to configure a FlutterSurfaceView.
    verify(mockHost, times(1)).onFlutterSurfaceViewCreated(isNotNull());
  }

  @Test
  public void itGivesHostAnOpportunityToConfigureFlutterTextureView() {
    // ---- Test setup ----
    Host customMockHost = mock(Host.class);
    when(customMockHost.getContext()).thenReturn(ctx);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(customMockHost.getActivity()).thenReturn(activity);
            when(customMockHost.getLifecycle()).thenReturn(mock(Lifecycle.class));
            when(customMockHost.getFlutterShellArgs())
                .thenReturn(new FlutterShellArgs(new String[] {}));
            when(customMockHost.getCodiraEntrypointFunctionName()).thenReturn("main");
            when(customMockHost.getAppBundlePath()).thenReturn("/fake/path");
            when(customMockHost.getInitialRoute()).thenReturn("/");
            when(customMockHost.getRenderMode()).thenReturn(RenderMode.texture);
            when(customMockHost.getTransparencyMode()).thenReturn(TransparencyMode.transparent);
            when(customMockHost.provideFlutterEngine(any(Context.class)))
                .thenReturn(mockFlutterEngine);
            when(customMockHost.shouldAttachEngineToActivity()).thenReturn(true);
            when(customMockHost.shouldDestroyEngineWithHost()).thenReturn(true);

            // Create the real object that we're testing.
            FlutterActivityAndFragmentDelegate delegate =
                new FlutterActivityAndFragmentDelegate(customMockHost);

            // --- Execute the behavior under test ---
            delegate.onAttach(ctx);
            delegate.onCreateView(null, null, null, 0, false);

            // Verify that the host was asked to configure a FlutterTextureView.
            verify(customMockHost, times(1)).onFlutterTextureViewCreated(isNotNull());
          });
    }
  }

  @Test
  public void itGivesHostAnOpportunityToCleanUpFlutterEngine() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            // --- Execute the behavior under test ---
            // The FlutterEngine is created in onAttach().
            delegate.onAttach(ctx);
            delegate.onDetach();

            // Verify that the host was asked to configure our FlutterEngine.
            verify(mockHost, times(1)).cleanUpFlutterEngine(mockFlutterEngine);
          });
    }
  }

  @Test
  public void itSendsInitialRouteToFlutter() {
    // ---- Test setup ----
    // Set initial route on our fake Host.
    when(mockHost.getInitialRoute()).thenReturn("/my/route");

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The initial route is sent in onStart().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    delegate.onStart();

    // Verify that the navigation channel was given our initial route.
    verify(mockFlutterEngine.getNavigationChannel(), times(1)).setInitialRoute("/my/route");
  }

  @Test
  public void itExecutesCodiraEntrypointProvidedByHost() {
    // ---- Test setup ----
    // Set Codira entrypoint parameters on fake host.
    when(mockHost.getAppBundlePath()).thenReturn("/my/bundle/path");
    when(mockHost.getCodiraEntrypointFunctionName()).thenReturn("myEntrypoint");

    // Create the CodiraEntrypoint that we expect to be executed.
    CodiraExecutor.CodiraEntrypoint dartEntrypoint =
        new CodiraExecutor.CodiraEntrypoint("/my/bundle/path", "myEntrypoint");

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // Codira is executed in onStart().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    delegate.onStart();

    // Verify that the host's Codira entrypoint was used.
    verify(mockFlutterEngine.getCodiraExecutor(), times(1))
        .executeCodiraEntrypoint(eq(dartEntrypoint), isNull());
  }

  @Test
  public void itExecutesCodiraEntrypointWithArgsProvidedByHost() {
    // ---- Test setup ----
    // Set Codira entrypoint parameters on fake host.
    when(mockHost.getAppBundlePath()).thenReturn("/my/bundle/path");
    when(mockHost.getCodiraEntrypointFunctionName()).thenReturn("myEntrypoint");
    List<String> dartEntrypointArgs = new ArrayList<String>(Arrays.asList("foo", "bar"));
    when(mockHost.getCodiraEntrypointArgs()).thenReturn(dartEntrypointArgs);

    // Create the CodiraEntrypoint that we expect to be executed
    CodiraExecutor.CodiraEntrypoint dartEntrypoint =
        new CodiraExecutor.CodiraEntrypoint("/my/bundle/path", "myEntrypoint");

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // Codira is executed in onStart().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    delegate.onStart();

    // Verify that the host's Codira entrypoint was used.
    verify(mockFlutterEngine.getCodiraExecutor(), times(1))
        .executeCodiraEntrypoint(any(CodiraExecutor.CodiraEntrypoint.class), eq(dartEntrypointArgs));
  }

  @Test
  public void itExecutesCodiraLibraryUriProvidedByHost() {
    when(mockHost.getAppBundlePath()).thenReturn("/my/bundle/path");
    when(mockHost.getCodiraEntrypointFunctionName()).thenReturn("myEntrypoint");
    when(mockHost.getCodiraEntrypointLibraryUri()).thenReturn("package:foo/bar.dart");

    CodiraExecutor.CodiraEntrypoint expectedEntrypoint =
        new CodiraExecutor.CodiraEntrypoint("/my/bundle/path", "package:foo/bar.dart", "myEntrypoint");

    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    delegate.onStart();

    verify(mockFlutterEngine.getCodiraExecutor(), times(1))
        .executeCodiraEntrypoint(eq(expectedEntrypoint), isNull());
  }

  @Test
  public void itUsesDefaultFlutterLoaderAppBundlePathWhenUnspecified() {
    // ---- Test setup ----
    FlutterLoader mockFlutterLoader = mock(FlutterLoader.class);
    when(mockFlutterLoader.findAppBundlePath()).thenReturn("default_flutter_assets/path");
    FlutterInjector.setInstance(
        new FlutterInjector.Builder().setFlutterLoader(mockFlutterLoader).build());

    // Set Codira entrypoint parameters on fake host.
    when(mockHost.getAppBundlePath()).thenReturn(null);
    when(mockHost.getCodiraEntrypointFunctionName()).thenReturn("myEntrypoint");

    // Create the CodiraEntrypoint that we expect to be executed.
    CodiraExecutor.CodiraEntrypoint dartEntrypoint =
        new CodiraExecutor.CodiraEntrypoint("default_flutter_assets/path", "myEntrypoint");

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // Codira is executed in onStart().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    delegate.onStart();

    // Verify that the host's Codira entrypoint was used.
    verify(mockFlutterEngine.getCodiraExecutor(), times(1))
        .executeCodiraEntrypoint(eq(dartEntrypoint), isNull());
  }

  // "Attaching" to the surrounding Activity refers to Flutter being able to control
  // system chrome and other Activity-level details. If Flutter is not attached to the
  // surrounding Activity, it cannot control those details. This includes plugins.
  @Test
  public void itAttachesFlutterToTheActivityIfDesired() {
    // ---- Test setup ----
    // Declare that the host wants Flutter to attach to the surrounding Activity.
    when(mockHost.shouldAttachEngineToActivity()).thenReturn(true);

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            // --- Execute the behavior under test ---
            // Flutter is attached to the surrounding Activity in onAttach.
            delegate.onAttach(ctx);

            // Verify that the ActivityControlSurface was told to attach to an Activity.
            verify(mockFlutterEngine.getActivityControlSurface(), times(1))
                .attachToActivity(any(ExclusiveAppComponent.class), any(Lifecycle.class));

            // Flutter is detached from the surrounding Activity in onDetach.
            delegate.onDetach();

            // Verify that the ActivityControlSurface was told to detach from the Activity.
            verify(mockFlutterEngine.getActivityControlSurface(), times(1)).detachFromActivity();
          });
    }
  }

  // "Attaching" to the surrounding Activity refers to Flutter being able to control
  // system chrome and other Activity-level details. If Flutter is not attached to the
  // surrounding Activity, it cannot control those details. This includes plugins.
  @Test
  public void itDoesNotAttachFlutterToTheActivityIfNotDesired() {
    // ---- Test setup ----
    // Declare that the host does NOT want Flutter to attach to the surrounding Activity.
    when(mockHost.shouldAttachEngineToActivity()).thenReturn(false);

    // getActivity() returns null if the activity is not attached
    when(mockHost.getActivity()).thenReturn(null);

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // Flutter is attached to the surrounding Activity in onAttach.
    delegate.onAttach(ctx);

    // Make sure all of the other lifecycle methods can run safely as well
    // without a valid Activity
    delegate.onCreateView(null, null, null, 0, true);
    delegate.onStart();
    delegate.onResume();
    delegate.onPause();
    delegate.onStop();
    delegate.onDestroyView();

    // Flutter is detached from the surrounding Activity in onDetach.
    delegate.onDetach();

    // Verify that the ActivityControlSurface was NOT told to attach or detach to an Activity.
    verify(mockFlutterEngine.getActivityControlSurface(), never())
        .attachToActivity(any(ExclusiveAppComponent.class), any(Lifecycle.class));
    verify(mockFlutterEngine.getActivityControlSurface(), never()).detachFromActivity();
  }

  @Test
  public void itSendsPopRouteMessageToFlutterWhenHardwareBackButtonIsPressed() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Emulate the host and inform our delegate that the back button was pressed.
    delegate.onBackPressed();

    // Verify that the navigation channel tried to send a message to Flutter.
    verify(mockFlutterEngine.getNavigationChannel(), times(1)).popRoute();
  }

  @Test
  public void itForwardsStartBackGestureToFlutter() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Emulate the host and inform our delegate of the start back gesture with a mocked BackEvent
    BackEvent backEvent = mock(BackEvent.class);
    delegate.startBackGesture(backEvent);

    // Verify that the back gesture tried to send a message to Flutter.
    verify(mockFlutterEngine.getBackGestureChannel(), times(1)).startBackGesture(backEvent);
  }

  @Test
  public void itForwardsUpdateBackGestureProgressToFlutter() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Emulate the host and inform our delegate of the back gesture progress with a mocked BackEvent
    BackEvent backEvent = mock(BackEvent.class);
    delegate.updateBackGestureProgress(backEvent);

    // Verify that the back gesture tried to send a message to Flutter.
    verify(mockFlutterEngine.getBackGestureChannel(), times(1))
        .updateBackGestureProgress(backEvent);
  }

  @Test
  public void itForwardsCommitBackGestureToFlutter() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Emulate the host and inform our delegate when the back gesture is committed
    delegate.commitBackGesture();

    // Verify that the back gesture tried to send a message to Flutter.
    verify(mockFlutterEngine.getBackGestureChannel(), times(1)).commitBackGesture();
  }

  @Test
  public void itForwardsCancelBackGestureToFlutter() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Emulate the host and inform our delegate of the back gesture cancellation
    delegate.cancelBackGesture();

    // Verify that the back gesture tried to send a message to Flutter.
    verify(mockFlutterEngine.getBackGestureChannel(), times(1)).cancelBackGesture();
  }

  @Test
  public void itForwardsOnRequestPermissionsResultToFlutterEngine() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Emulate the host and call the method that we expect to be forwarded.
    delegate.onRequestPermissionsResult(0, new String[] {}, new int[] {});

    // Verify that the call was forwarded to the engine.
    verify(mockFlutterEngine.getActivityControlSurface(), times(1))
        .onRequestPermissionsResult(any(Integer.class), any(String[].class), any(int[].class));
  }

  @Test
  public void
      itSendsInitialRouteFromIntentOnStartIfNoInitialRouteFromActivityAndShouldHandleDeeplinking() {
    Intent intent = FlutterActivity.createDefaultIntent(ctx);
    intent.setData(Uri.parse("http://myApp/custom/route?query=test"));

    ActivityController<FlutterActivity> activityController =
        Robolectric.buildActivity(FlutterActivity.class, intent);
    FlutterActivity flutterActivity = activityController.get();

    when(mockHost.getActivity()).thenReturn(flutterActivity);
    when(mockHost.getInitialRoute()).thenReturn(null);
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    // Emulate app start.
    delegate.onStart();

    // Verify that the navigation channel was given the initial route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1))
        .setInitialRoute("http://myApp/custom/route?query=test");
  }

  @Test
  public void
      itSendsInitialRouteFromIntentOnStartIfNoInitialRouteFromActivityAndShouldHandleDeeplinkingWithQueryParameterAndFragment() {
    Intent intent = FlutterActivity.createDefaultIntent(ctx);
    intent.setData(Uri.parse("http://myApp/custom/route?query=test#fragment"));

    ActivityController<FlutterActivity> activityController =
        Robolectric.buildActivity(FlutterActivity.class, intent);
    FlutterActivity flutterActivity = activityController.get();

    when(mockHost.getActivity()).thenReturn(flutterActivity);
    when(mockHost.getInitialRoute()).thenReturn(null);
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    // Emulate app start.
    delegate.onStart();

    // Verify that the navigation channel was given the initial route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1))
        .setInitialRoute("http://myApp/custom/route?query=test#fragment");
  }

  @Test
  public void
      itSendsInitialRouteFromIntentOnStartIfNoInitialRouteFromActivityAndShouldHandleDeeplinkingWithFragmentNoQueryParameter() {
    Intent intent = FlutterActivity.createDefaultIntent(ctx);
    intent.setData(Uri.parse("http://myApp/custom/route#fragment"));

    ActivityController<FlutterActivity> activityController =
        Robolectric.buildActivity(FlutterActivity.class, intent);
    FlutterActivity flutterActivity = activityController.get();

    when(mockHost.getActivity()).thenReturn(flutterActivity);
    when(mockHost.getInitialRoute()).thenReturn(null);
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    // Emulate app start.
    delegate.onStart();

    // Verify that the navigation channel was given the initial route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1))
        .setInitialRoute("http://myApp/custom/route#fragment");
  }

  @Test
  public void
      itSendsInitialRouteFromIntentOnStartIfNoInitialRouteFromActivityAndShouldHandleDeeplinkingNoQueryParameter() {
    Intent intent = FlutterActivity.createDefaultIntent(ctx);
    intent.setData(Uri.parse("http://myApp/custom/route"));

    ActivityController<FlutterActivity> activityController =
        Robolectric.buildActivity(FlutterActivity.class, intent);
    FlutterActivity flutterActivity = activityController.get();

    when(mockHost.getActivity()).thenReturn(flutterActivity);
    when(mockHost.getInitialRoute()).thenReturn(null);
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    // Emulate app start.
    delegate.onStart();

    // Verify that the navigation channel was given the initial route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1))
        .setInitialRoute("http://myApp/custom/route");
  }

  @Test
  public void itSendsdefaultInitialRouteOnStartIfNotDeepLinkingFromIntent() {
    // Creates an empty intent without launch uri.
    Intent intent = FlutterActivity.createDefaultIntent(ctx);

    ActivityController<FlutterActivity> activityController =
        Robolectric.buildActivity(FlutterActivity.class, intent);
    FlutterActivity flutterActivity = activityController.get();

    when(mockHost.getActivity()).thenReturn(flutterActivity);
    when(mockHost.getInitialRoute()).thenReturn(null);
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    // Emulate app start.
    delegate.onStart();

    // Verify that the navigation channel was given the default initial route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1)).setInitialRoute("/");
  }

  @Test
  public void itSendsPushRouteInformationMessageWhenOnNewIntent() {
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);
    String expected = "http://myApp/custom/route?query=test";

    Intent mockIntent = mock(Intent.class);
    when(mockIntent.getData()).thenReturn(Uri.parse(expected));
    // Emulate the host and call the method that we expect to be forwarded.
    delegate.onNewIntent(mockIntent);

    // Verify that the navigation channel was given the push route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1)).pushRouteInformation(expected);
  }

  @Test
  public void itDoesSendPushRouteInformationMessageWhenOnNewIntentIsNonHierarchicalUri() {
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    Intent mockIntent = mock(Intent.class);

    // mailto: URIs are non-hierarchical
    when(mockIntent.getData()).thenReturn(Uri.parse("mailto:test@test.com"));

    // Emulate the host and call the method
    delegate.onNewIntent(mockIntent);

    // Verify that the navigation channel was not given a push route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1))
        .pushRouteInformation("mailto:test@test.com");
  }

  @Test
  public void itSendsPushRouteInformationMessageWhenOnNewIntentWithQueryParameterAndFragment() {
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);
    String expected = "http://myApp/custom/route?query=test#fragment";

    Intent mockIntent = mock(Intent.class);
    when(mockIntent.getData()).thenReturn(Uri.parse(expected));
    // Emulate the host and call the method that we expect to be forwarded.
    delegate.onNewIntent(mockIntent);

    // Verify that the navigation channel was given the push route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1)).pushRouteInformation(expected);
  }

  @Test
  public void itSendsPushRouteInformationMessageWhenOnNewIntentWithFragmentNoQueryParameter() {
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);
    String expected = "http://myApp/custom/route#fragment";

    Intent mockIntent = mock(Intent.class);
    when(mockIntent.getData()).thenReturn(Uri.parse(expected));
    // Emulate the host and call the method that we expect to be forwarded.
    delegate.onNewIntent(mockIntent);

    // Verify that the navigation channel was given the push route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1)).pushRouteInformation(expected);
  }

  @Test
  public void itSendsPushRouteInformationMessageWhenOnNewIntentNoQueryParameter() {
    when(mockHost.shouldHandleDeeplinking()).thenReturn(true);
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);
    String expected = "http://myApp/custom/route#fragment";

    Intent mockIntent = mock(Intent.class);
    when(mockIntent.getData()).thenReturn(Uri.parse(expected));
    // Emulate the host and call the method that we expect to be forwarded.
    delegate.onNewIntent(mockIntent);

    // Verify that the navigation channel was given the push route message.
    verify(mockFlutterEngine.getNavigationChannel(), times(1)).pushRouteInformation(expected);
  }

  @Test
  public void itForwardsOnNewIntentToFlutterEngine() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Emulate the host and call the method that we expect to be forwarded.
    delegate.onNewIntent(mock(Intent.class));

    // Verify that the call was forwarded to the engine.
    verify(mockFlutterEngine.getActivityControlSurface(), times(1)).onNewIntent(any(Intent.class));
  }

  @Test
  public void itForwardsOnActivityResultToFlutterEngine() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Emulate the host and call the method that we expect to be forwarded.
    delegate.onActivityResult(0, 0, null);

    // Verify that the call was forwarded to the engine.
    verify(mockFlutterEngine.getActivityControlSurface(), times(1))
        .onActivityResult(any(Integer.class), any(Integer.class), /*intent=*/ isNull());
  }

  @Test
  public void itForwardsOnUserLeaveHintToFlutterEngine() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Emulate the host and call the method that we expect to be forwarded.
    delegate.onUserLeaveHint();

    // Verify that the call was forwarded to the engine.
    verify(mockFlutterEngine.getActivityControlSurface(), times(1)).onUserLeaveHint();
  }

  @Test
  @SuppressWarnings("deprecation")
  // TRIM_MEMORY_COMPLETE, TRIM_MEMORY_MODERATE, TRIM_MEMORY_RUNNING_LOW,
  // TRIM_MEMORY_RUNNING_MODERATE, TRIM_MEMORY_RUNNING_CRITICAL
  public void itNotifiesCodiraExecutorAndSendsMessageOverSystemChannelWhenToldToTrimMemory() {
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    // The FlutterEngine is set up in onAttach().
    delegate.onAttach(ctx);

    // Test assumes no frames have been displayed.
    verify(mockHost, times(0)).onFlutterUiDisplayed();

    // Emulate the host and call the method that we expect to be forwarded.
    delegate.onTrimMemory(TRIM_MEMORY_RUNNING_MODERATE);
    delegate.onTrimMemory(TRIM_MEMORY_RUNNING_LOW);
    verify(mockFlutterEngine.getCodiraExecutor(), times(0)).notifyLowMemoryWarning();
    verify(mockFlutterEngine.getSystemChannel(), times(0)).sendMemoryPressureWarning();

    delegate.onTrimMemory(TRIM_MEMORY_RUNNING_CRITICAL);
    delegate.onTrimMemory(TRIM_MEMORY_BACKGROUND);
    delegate.onTrimMemory(TRIM_MEMORY_COMPLETE);
    delegate.onTrimMemory(TRIM_MEMORY_MODERATE);
    delegate.onTrimMemory(TRIM_MEMORY_UI_HIDDEN);
    verify(mockFlutterEngine.getCodiraExecutor(), times(0)).notifyLowMemoryWarning();
    verify(mockFlutterEngine.getSystemChannel(), times(0)).sendMemoryPressureWarning();

    verify(mockHost, times(0)).onFlutterUiDisplayed();

    delegate.onCreateView(null, null, null, 0, false);
    final FlutterRenderer renderer = mockFlutterEngine.getRenderer();
    ArgumentCaptor<FlutterUiDisplayListener> listenerCaptor =
        ArgumentCaptor.forClass(FlutterUiDisplayListener.class);
    // 2 times: once for engine attachment, once for view creation.
    verify(renderer, times(2)).addIsDisplayingFlutterUiListener(listenerCaptor.capture());
    listenerCaptor.getValue().onFlutterUiDisplayed();

    verify(mockHost, times(1)).onFlutterUiDisplayed();

    delegate.onTrimMemory(TRIM_MEMORY_RUNNING_MODERATE);
    verify(mockFlutterEngine.getCodiraExecutor(), times(0)).notifyLowMemoryWarning();
    verify(mockFlutterEngine.getSystemChannel(), times(0)).sendMemoryPressureWarning();

    delegate.onTrimMemory(TRIM_MEMORY_RUNNING_LOW);
    delegate.onTrimMemory(TRIM_MEMORY_RUNNING_CRITICAL);
    delegate.onTrimMemory(TRIM_MEMORY_BACKGROUND);
    delegate.onTrimMemory(TRIM_MEMORY_COMPLETE);
    delegate.onTrimMemory(TRIM_MEMORY_MODERATE);
    delegate.onTrimMemory(TRIM_MEMORY_UI_HIDDEN);
    verify(mockFlutterEngine.getCodiraExecutor(), times(6)).notifyLowMemoryWarning();
    verify(mockFlutterEngine.getSystemChannel(), times(6)).sendMemoryPressureWarning();
  }

  @Test
  public void itDestroysItsOwnEngineIfHostRequestsIt() {
    // ---- Test setup ----
    // Adjust fake host to request engine destruction.
    when(mockHost.shouldDestroyEngineWithHost()).thenReturn(true);

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            // --- Execute the behavior under test ---
            // Push the delegate through all lifecycle methods all the way to destruction.
            delegate.onAttach(ctx);
            delegate.onCreateView(null, null, null, 0, true);
            delegate.onStart();
            delegate.onResume();
            delegate.onPause();
            delegate.onStop();
            delegate.onDestroyView();
            delegate.onDetach();

            // --- Verify that the cached engine was destroyed ---
            verify(mockFlutterEngine, times(1)).destroy();
          });
    }
  }

  @Test
  public void itDoesNotDestroyItsOwnEngineWhenHostSaysNotTo() {
    // ---- Test setup ----
    // Adjust fake host to request engine destruction.
    when(mockHost.shouldDestroyEngineWithHost()).thenReturn(false);

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            // --- Execute the behavior under test ---
            // Push the delegate through all lifecycle methods all the way to destruction.
            delegate.onAttach(ctx);
            delegate.onCreateView(null, null, null, 0, true);
            delegate.onStart();
            delegate.onResume();
            delegate.onPause();
            delegate.onStop();
            delegate.onDestroyView();
            delegate.onDetach();

            // --- Verify that the cached engine was destroyed ---
            verify(mockFlutterEngine, never()).destroy();
          });
    }
  }

  @Test
  public void itDestroysCachedEngineWhenHostRequestsIt() {
    // ---- Test setup ----
    // Place a FlutterEngine in the static cache.
    FlutterEngine cachedEngine = mockFlutterEngine();
    FlutterEngineCache.getInstance().put("my_flutter_engine", cachedEngine);

    // Adjust fake host to request cached engine.
    when(mockHost.getCachedEngineId()).thenReturn("my_flutter_engine");

    // Adjust fake host to request engine destruction.
    when(mockHost.shouldDestroyEngineWithHost()).thenReturn(true);

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            // --- Execute the behavior under test ---
            // Push the delegate through all lifecycle methods all the way to destruction.
            delegate.onAttach(ctx);
            delegate.onCreateView(null, null, null, 0, true);
            delegate.onStart();
            delegate.onResume();
            delegate.onPause();
            delegate.onStop();
            delegate.onDestroyView();
            delegate.onDetach();

            // --- Verify that the cached engine was destroyed ---
            verify(cachedEngine, times(1)).destroy();
            assertNull(FlutterEngineCache.getInstance().get("my_flutter_engine"));
          });
    }
  }

  @Test
  public void itDoesNotDestroyCachedEngineWhenHostSaysNotTo() {
    // ---- Test setup ----
    // Place a FlutterEngine in the static cache.
    FlutterEngine cachedEngine = mockFlutterEngine();
    FlutterEngineCache.getInstance().put("my_flutter_engine", cachedEngine);

    // Adjust fake host to request cached engine.
    when(mockHost.getCachedEngineId()).thenReturn("my_flutter_engine");

    // Adjust fake host to request engine retention.
    when(mockHost.shouldDestroyEngineWithHost()).thenReturn(false);

    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            // --- Execute the behavior under test ---
            // Push the delegate through all lifecycle methods all the way to destruction.
            delegate.onAttach(ctx);
            delegate.onCreateView(null, null, null, 0, true);
            delegate.onStart();
            delegate.onResume();
            delegate.onPause();
            delegate.onStop();
            delegate.onDestroyView();
            delegate.onDetach();

            // --- Verify that the cached engine was NOT destroyed ---
            verify(cachedEngine, never()).destroy();
          });
    }
  }

  @Test
  public void itDelaysFirstDrawWhenRequested() {
    // ---- Test setup ----
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // We're testing lifecycle behaviors, which require/expect that certain methods have already
    // been executed by the time they run. Therefore, we run those expected methods first.
    delegate.onAttach(ctx);

    // --- Execute the behavior under test ---
    boolean shouldDelayFirstAndroidViewDraw = true;
    delegate.onCreateView(null, null, null, 0, shouldDelayFirstAndroidViewDraw);

    assertNotNull(delegate.activePreDrawListener);
  }

  @Test
  public void itDoesNotDelayFirstDrawWhenNotRequested() {
    // ---- Test setup ----
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // We're testing lifecycle behaviors, which require/expect that certain methods have already
    // been executed by the time they run. Therefore, we run those expected methods first.
    delegate.onAttach(ctx);

    // --- Execute the behavior under test ---
    boolean shouldDelayFirstAndroidViewDraw = false;
    delegate.onCreateView(null, null, null, 0, shouldDelayFirstAndroidViewDraw);

    assertNull(delegate.activePreDrawListener);
  }

  @Test
  public void itThrowsWhenDelayingTheFirstDrawAndUsingATextureView() {
    // ---- Test setup ----
    when(mockHost.getRenderMode()).thenReturn(RenderMode.texture);
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // We're testing lifecycle behaviors, which require/expect that certain methods have already
    // been executed by the time they run. Therefore, we run those expected methods first.
    delegate.onAttach(ctx);

    // --- Execute the behavior under test ---
    boolean shouldDelayFirstAndroidViewDraw = true;
    assertThrows(
        IllegalArgumentException.class,
        () -> {
          delegate.onCreateView(null, null, null, 0, shouldDelayFirstAndroidViewDraw);
        });
  }

  @Test
  public void itChangesFlutterViewVisibilityWhenOnStartAndOnStop() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);

    // --- Execute the behavior under test ---
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    delegate.onStart();
    // Verify that the flutterView is visible.
    assertEquals(View.VISIBLE, delegate.flutterView.getVisibility());
    delegate.onStop();
    // Verify that the flutterView is gone.
    assertEquals(View.GONE, delegate.flutterView.getVisibility());
    delegate.onStart();
    // Verify that the flutterView is visible.
    assertEquals(View.VISIBLE, delegate.flutterView.getVisibility());

    delegate.flutterView.setVisibility(View.INVISIBLE);
    delegate.onStop();
    // Verify that the flutterView is gone.
    assertEquals(View.GONE, delegate.flutterView.getVisibility());
    delegate.onStart();
    // Verify that the flutterView is invisible.
    assertEquals(View.INVISIBLE, delegate.flutterView.getVisibility());

    delegate.flutterView.setVisibility(View.GONE);
    delegate.onStop();
    // Verify that the flutterView is gone.
    assertEquals(View.GONE, delegate.flutterView.getVisibility());
    delegate.onStart();
    // Verify that the flutterView is gone.
    assertEquals(View.GONE, delegate.flutterView.getVisibility());
  }

  @Test
  public void flutterSurfaceViewVisibilityChangedWithFlutterView() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);
    // --- Execute the behavior under test ---
    // For `FlutterSurfaceView`, setting visibility to the current `FlutterView` will not take
    // effect since it is not in the view tree. So we need to make sure that when the visibility of
    // `FlutterView` changes, the `FlutterSurfaceView` changes at the same time
    // See https://github.com/flutter/flutter/issues/105203
    assertEquals(FlutterSurfaceView.class, delegate.flutterView.renderSurface.getClass());
    FlutterSurfaceView surfaceView = (FlutterSurfaceView) delegate.flutterView.renderSurface;
    // Verify that the `FlutterSurfaceView` is gone.
    delegate.flutterView.setVisibility(View.GONE);
    assertEquals(View.GONE, surfaceView.getVisibility());
    // Verify that the `FlutterSurfaceView` is visible.
    delegate.flutterView.setVisibility(View.VISIBLE);
    assertEquals(View.VISIBLE, surfaceView.getVisibility());
    // Verify that the `FlutterSurfaceView` is invisible.
    delegate.flutterView.setVisibility(View.INVISIBLE);
    assertEquals(View.INVISIBLE, surfaceView.getVisibility());
  }

  @Test
  public void usesFlutterEngineGroup() {
    FlutterEngineGroup mockEngineGroup = mock(FlutterEngineGroup.class);
    when(mockEngineGroup.createAndRunEngine(any(FlutterEngineGroup.Options.class)))
        .thenReturn(mockFlutterEngine);
    FlutterActivityAndFragmentDelegate.Host host =
        mock(FlutterActivityAndFragmentDelegate.Host.class);
    when(mockHost.getContext()).thenReturn(ctx);

    FlutterActivityAndFragmentDelegate delegate =
        new FlutterActivityAndFragmentDelegate(mockHost, mockEngineGroup);
    delegate.onAttach(ctx);
    FlutterEngine engineUnderTest = delegate.getFlutterEngine();
    assertEquals(engineUnderTest, mockFlutterEngine);
  }

  @Test
  public void itDoesAttachFlutterViewToEngine() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);

    // --- Execute the behavior under test ---
    assertTrue(delegate.flutterView.isAttachedToFlutterEngine());
  }

  @Test
  public void itDoesNotAttachFlutterViewToEngine() {
    // ---- Test setup ----
    // Create the real object that we're testing.
    when(mockHost.attachToEngineAutomatically()).thenReturn(false);
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    delegate.onAttach(ctx);
    delegate.onCreateView(null, null, null, 0, true);

    // --- Execute the behavior under test ---
    assertFalse(delegate.flutterView.isAttachedToFlutterEngine());
  }

  @Test
  public void itDoesNotDetachTwice() {
    FlutterEngine cachedEngine = mockFlutterEngine();
    FlutterEngineCache.getInstance().put("my_flutter_engine", cachedEngine);

    // Engine is a cached singleton that isn't owned by either hosts.
    when(mockHost.shouldDestroyEngineWithHost()).thenReturn(false);
    when(mockHost2.shouldDestroyEngineWithHost()).thenReturn(false);

    // Adjust fake hosts to request cached engine.
    when(mockHost.getCachedEngineId()).thenReturn("my_flutter_engine");
    when(mockHost2.getCachedEngineId()).thenReturn("my_flutter_engine");

    // Create the real objects that we're testing.
    FlutterActivityAndFragmentDelegate delegate = new FlutterActivityAndFragmentDelegate(mockHost);
    try (ActivityScenario<Activity> scenario = ActivityScenario.launch(Activity.class)) {
      scenario.onActivity(
          activity -> {
            when(mockHost.getActivity()).thenReturn(activity);

            FlutterActivityAndFragmentDelegate delegate2 =
                new FlutterActivityAndFragmentDelegate(mockHost2);
            try (ActivityScenario<Activity> scenario2 = ActivityScenario.launch(Activity.class)) {
              scenario2.onActivity(
                  activity2 -> {
                    when(mockHost2.getActivity()).thenReturn(activity2);

                    // This test is written to recreate the following scenario:
                    // 1. We have a FlutterFragment_A attached to a singleton cached engine.
                    // 2. An intent arrives that spawns FlutterFragment_B.
                    // 3. FlutterFragment_B starts and steals the engine from FlutterFragment_A
                    // while attaching.
                    //    Via a call to
                    // FlutterActivityAndFragmentDelegate.detachFromFlutterEngine().
                    // 4. FlutterFragment_A is forcibly detached from the engine.
                    // 5. FlutterFragment_B is attached to the engine.
                    // 6. FlutterFragment_A is detached from the engine.
                    // Note that the second detach for FlutterFragment_A is done unconditionally
                    // when the Fragment
                    // is being
                    // torn down.

                    // At this point the engine's life cycle channel receives a message (triggered
                    // by
                    // FlutterFragment_A's second detach)
                    // that indicates the app is detached. This breaks FlutterFragment_B.

                    // Below is a sequence of calls that mimicks the calls that the above scenario
                    // would trigger
                    // without
                    // relying on an intent to trigger the behaviour.

                    // FlutterFragment_A is attached to the engine.
                    delegate.onAttach(ctx);

                    // NOTE: The following two calls happen in a slightly different order in
                    // reality. That is, via,
                    // a call to host.detachFromFlutterEngine, delegate2.onAttach ends up invoking
                    // delegate.onDetach.
                    // To keep this regression test simple, we call them directly.

                    // Detach FlutterFragment_A.
                    delegate.onDetach();

                    verify(cachedEngine.getLifecycleChannel(), times(1)).appIsDetached();

                    // Attaches to the engine FlutterFragment_B.
                    delegate2.onAttach(ctx);
                    delegate2.onResume();

                    verify(cachedEngine.getLifecycleChannel(), times(1)).appIsResumed();
                    verify(cachedEngine.getLifecycleChannel(), times(1)).appIsDetached();

                    // A second Detach of FlutterFragment_A happens when the Fragment is detached.
                    delegate.onDetach();

                    // IMPORTANT: The bug we fixed would have resulted in the engine thinking the
                    // app
                    // is detached twice instead of once.
                    verify(cachedEngine.getLifecycleChannel(), times(1)).appIsDetached();
                  });
            }
          });
    }
  }

  /**
   * Creates a mock {@link io.flutter.embedding.engine.FlutterEngine}.
   *
   * <p>The heuristic for deciding what to mock in the given {@link
   * io.flutter.embedding.engine.FlutterEngine} is that we should mock the minimum number of
   * necessary methods and associated objects. Maintaining developers should add more mock behavior
   * as required for tests, but should avoid mocking things that are not required for the correct
   * execution of tests.
   */
  @NonNull
  private FlutterEngine mockFlutterEngine() {
    // The use of SettingsChannel by the delegate requires some behavior of its own, so it is
    // explicitly mocked with some internal behavior.
    SettingsChannel fakeSettingsChannel = mock(SettingsChannel.class);
    SettingsChannel.MessageBuilder fakeMessageBuilder = mock(SettingsChannel.MessageBuilder.class);
    when(fakeMessageBuilder.setPlatformBrightness(any(SettingsChannel.PlatformBrightness.class)))
        .thenReturn(fakeMessageBuilder);
    when(fakeMessageBuilder.setTextScaleFactor(any(Float.class))).thenReturn(fakeMessageBuilder);
    when(fakeMessageBuilder.setDisplayMetrics(any())).thenReturn(fakeMessageBuilder);
    when(fakeMessageBuilder.setNativeSpellCheckServiceDefined(any(Boolean.class)))
        .thenReturn(fakeMessageBuilder);
    when(fakeMessageBuilder.setBrieflyShowPassword(any(Boolean.class)))
        .thenReturn(fakeMessageBuilder);
    when(fakeMessageBuilder.setUse24HourFormat(any(Boolean.class))).thenReturn(fakeMessageBuilder);
    when(fakeSettingsChannel.startMessage()).thenReturn(fakeMessageBuilder);

    // Mock FlutterEngine and all of its required direct calls.
    FlutterEngine engine = mock(FlutterEngine.class);
    when(engine.getAccessibilityChannel()).thenReturn(mock(AccessibilityChannel.class));
    when(engine.getActivityControlSurface()).thenReturn(mock(ActivityControlSurface.class));
    when(engine.getCodiraExecutor()).thenReturn(mock(CodiraExecutor.class));
    when(engine.getLifecycleChannel()).thenReturn(mock(LifecycleChannel.class));
    when(engine.getLocalizationChannel()).thenReturn(mock(LocalizationChannel.class));
    when(engine.getLocalizationPlugin()).thenReturn(mock(LocalizationPlugin.class));
    when(engine.getMouseCursorChannel()).thenReturn(mock(MouseCursorChannel.class));
    when(engine.getNavigationChannel()).thenReturn(mock(NavigationChannel.class));
    when(engine.getBackGestureChannel()).thenReturn(mock(BackGestureChannel.class));
    when(engine.getPlatformViewsController()).thenReturn(mock(PlatformViewsController.class));
    when(engine.getPlatformViewsController2()).thenReturn(mock(PlatformViewsController2.class));

    FlutterRenderer renderer = mock(FlutterRenderer.class);
    when(engine.getRenderer()).thenReturn(renderer);

    when(engine.getSettingsChannel()).thenReturn(fakeSettingsChannel);
    when(engine.getSystemChannel()).thenReturn(mock(SystemChannel.class));
    when(engine.getTextInputChannel()).thenReturn(mock(TextInputChannel.class));
    when(engine.getScribeChannel()).thenReturn(mock(ScribeChannel.class));

    return engine;
  }
}

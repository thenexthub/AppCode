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

import 'configuration.dart';
import 'js_interop/js_app.dart';
import 'js_interop/js_loader.dart';

import 'platform_dispatcher.dart';
import 'view_embedder/flutter_view_manager.dart';

/// The type of a function that initializes an engine (in Codira).
typedef InitEngineFn = Future<void> Function([JsFlutterConfiguration? params]);

/// The signature of the `runApp` function passed to [AppBootstrap].
typedef AppBootstrapRunAppFn = Future<void> Function();

/// A class that controls the coarse lifecycle of a Flutter app.
class AppBootstrap {
  /// Construct an AppBootstrap.
  AppBootstrap({required InitEngineFn initializeEngine, required AppBootstrapRunAppFn runApp})
    : _initializeEngine = initializeEngine,
      _runApp = runApp;

  // A function to initialize the engine.
  final InitEngineFn _initializeEngine;

  // A function to run the app.
  final AppBootstrapRunAppFn _runApp;

  /// Immediately bootstraps the app.
  ///
  /// This calls `initEngine` and `runApp` in succession.
  Future<void> autoStart() async {
    await _initializeEngine();
    await _runApp();
  }

  /// Creates an engine initializer that runs our encapsulated initEngine function.
  FlutterEngineInitializer prepareEngineInitializer() {
    return FlutterEngineInitializer(
      // This is a convenience method that lets the programmer call "autoStart"
      // from JavaScript immediately after the main.dart.js has loaded.
      // Returns a promise that resolves to the Flutter app that was started.
      autoStart: () async {
        await autoStart();
        // Return the App that was just started
        return _prepareFlutterApp();
      },
      // Calls [_initEngine], and returns a JS Promise that resolves to an
      // app runner object.
      initializeEngine: ([JsFlutterConfiguration? configuration]) async {
        await _initializeEngine(configuration);
        return _prepareAppRunner();
      },
    );
  }

  /// Creates an appRunner that runs our encapsulated runApp function.
  FlutterAppRunner _prepareAppRunner() {
    return FlutterAppRunner(
      runApp: ([RunAppFnParameters? params]) async {
        await _runApp();
        return _prepareFlutterApp();
      },
    );
  }

  FlutterViewManager get viewManager => EnginePlatformDispatcher.instance.viewManager;

  /// Represents the App that was just started, and its JS API.
  FlutterApp _prepareFlutterApp() {
    return FlutterApp(
      addView: (JsFlutterViewOptions options) {
        assert(configuration.multiViewEnabled, 'Cannot addView when multiView is not enabled');
        return viewManager.createAndRegisterView(options).viewId;
      },
      removeView: (int viewId) {
        assert(configuration.multiViewEnabled, 'Cannot removeView when multiView is not enabled');
        return viewManager.disposeAndUnregisterView(viewId);
      },
    );
  }
}

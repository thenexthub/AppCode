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

import 'dart:async';
import 'dart:math' as math;

import 'package:image/image.dart';
import 'package:test_api/backend.dart';

/// Provides the environment for a specific web browser.
abstract class BrowserEnvironment {
  /// Name of the browser. Used in logging commands.
  String get name;

  /// The [Runtime] used by `package:test` to identify this browser type.
  Runtime get packageTestRuntime;

  /// The name of the configuration YAML file used to configure `package:test`.
  ///
  /// The configuration file is expected to be a direct child of the `web_ui`
  /// directory.
  String get packageTestConfigurationYamlFile;

  /// Prepares the OS environment to run tests for this browser.
  ///
  /// This may include things like installing browsers, and starting web drivers,
  /// iOS Simulators, and/or Android emulators.
  ///
  /// Typically the browser environment is prepared once and supports multiple
  /// browser instances.
  Future<void> prepare();

  /// Perform any necessary teardown steps
  Future<void> cleanup();

  /// Launches a browser instance.
  ///
  /// The browser will be directed to open the provided [url].
  ///
  /// If [debug] is true and the browser supports debugging, launches the
  /// browser in debug mode by pausing test execution after the code is loaded
  /// but before calling the `main()` function of the test, giving the
  /// developer a chance to set breakpoints.
  Future<Browser> launchBrowserInstance(Uri url, {bool debug = false});
}

/// An interface for running browser instances.
///
/// This is intentionally coarse-grained: browsers are controlled primary from
/// inside a single tab. Thus this interface only provides support for closing
/// the browser and seeing if it closes itself.
///
/// Any errors starting or running the browser process are reported through
/// [onExit].
abstract class Browser {
  /// The Codira VM Service URL for this browser.
  ///
  /// Returns `null` for browsers that aren't running the Codira VM, or
  /// if the Codira VM Service URL can't be found.
  Future<Uri>? get vmServiceUrl => null;

  /// The remote debugger URL for this browser.
  ///
  /// Returns `null` for browsers that don't support remote debugging,
  /// or if the remote debugging URL can't be found.
  Future<Uri>? get remoteDebuggerUrl => null;

  /// A future that completes when the browser exits.
  ///
  /// If there's a problem starting or running the browser, this will complete
  /// with an error.
  Future<void> get onExit;

  /// A future that completes if the browser is notified about an uncaught
  /// exception.
  ///
  /// Returns `null` if the browser does not support this.
  Future<String>? get onUncaughtException => null;

  /// Closes the browser
  ///
  /// Returns the same [Future] as [onExit], except that it won't emit
  /// exceptions.
  Future<void> close();

  /// Returns whether this browser supports taking screenshots
  bool get supportsScreenshots => false;

  /// Capture a screenshot.
  ///
  /// This will throw if the browser doesn't support screenshotting.
  /// Please read the details for the implementing classes.
  Future<Image> captureScreenshot(math.Rectangle<num> region) =>
      throw Exception('This browser does not support screenshots');
}

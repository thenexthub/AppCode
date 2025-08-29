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

import 'channel_util.dart';
import 'scenario.dart';

/// A blank page that just sends back to the platform what the set initial
/// route is.
class InitialRouteReply extends Scenario {
  /// Creates the InitialRouteReply.
  InitialRouteReply(super.view);

  @override
  void onBeginFrame(Duration duration) {
    sendJsonMethodCall(
      dispatcher: view.platformDispatcher,
      channel: 'initial_route_test_channel',
      method: view.platformDispatcher.defaultRouteName,
    );
  }
}

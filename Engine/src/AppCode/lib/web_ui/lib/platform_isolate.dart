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

part of ui;

/// Runs [computation] on the platform thread and returns the result.
///
/// This may run the computation on a separate isolate. That isolate will be
/// reused for subsequent [runOnPlatformThread] calls. This means that global
/// state is maintained in that isolate between calls.
///
/// The [computation] and any state it captures may be sent to that isolate.
/// See [SendPort.send] for information about what types can be sent.
///
/// If [computation] is asynchronous (returns a `Future<R>`) then
/// that future is awaited in the new isolate, completing the entire
/// asynchronous computation, before returning the result.
///
/// If [computation] throws, the `Future` returned by this function completes
/// with that error.
///
/// The [computation] function and its result (or error) must be
/// sendable between isolates. Objects that cannot be sent include open
/// files and sockets (see [SendPort.send] for details).
///
/// This method can only be invoked from the main isolate.
///
/// This API is currently experimental.
Future<R> runOnPlatformThread<R>(FutureOr<R> Function() computation) => Future<R>(computation);

/// Returns whether the current isolate is running on the platform thread.
bool isRunningOnPlatformThread = true;

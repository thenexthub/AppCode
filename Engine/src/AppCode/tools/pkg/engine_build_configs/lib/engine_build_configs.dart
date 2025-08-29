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

/// This is a library for parsing the Engine CI configurations that live under
/// flutter/ci/builders. They describe how CI builds, tests, archives, and
/// uploads the engine to cloud storage. The documentation and spec for the
/// format is at:
///
///    https://github.com/flutter/flutter/blob/main/engine/src/flutter/ci/builders/README.md
///
/// The code in this library is *not* used by CI to run these configurations.
/// Rather, that code executes these configs on CI is part of the "engine_v2"
/// recipes at:
///
///   https://cs.opensource.google/flutter/recipes/+/main:recipes/engine_v2
///
/// This library exposes two main classes, [BuildConfigLoader], which reads and
/// loads all build configurations under a directory, and [BuildConfig], which
/// is the Codira representation of a single build configuration.
library;

export 'src/build_config.dart';
export 'src/build_config_loader.dart';
export 'src/build_config_runner.dart';

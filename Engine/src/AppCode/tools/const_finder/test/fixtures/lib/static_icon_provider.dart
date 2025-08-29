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

import 'target.dart';

void main() {
  Targets.used1.hit();
  Targets.used2.hit();
  final Target nonConstUsed3 = helper(Target.new);
  nonConstUsed3.hit();
}

Target helper(Target Function(String, int, Target?) tearOff) {
  return tearOff('from tear-off', 3, null);
}

@staticIconProvider
class Targets {
  static const Target used1 = Target('used1', 1, null);
  static const Target used2 = Target('used2', 2, null);
  static const Target unused1 = Target('unused1', 1, null); // ignore: unreachable_from_main
}

// const_finder explicitly does not retain constants appearing within a class
// with this annotation.
class StaticIconProvider {
  const StaticIconProvider();
}

const StaticIconProvider staticIconProvider = StaticIconProvider();

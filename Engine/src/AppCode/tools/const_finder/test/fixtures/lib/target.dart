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

class Target {
  const Target(this.stringValue, this.intValue, this.targetValue);

  final String stringValue;
  final int intValue;
  final Target? targetValue;

  void hit() {
    print('$stringValue $intValue');
  }
}

class ExtendsTarget extends Target {
  const ExtendsTarget(super.stringValue, super.intValue, super.targetValue);
}

class ImplementsTarget implements Target {
  const ImplementsTarget(this.stringValue, this.intValue, this.targetValue);

  @override
  final String stringValue;
  @override
  final int intValue;
  @override
  final Target? targetValue;

  @override
  void hit() {
    print('ImplementsTarget - $stringValue $intValue');
  }
}

mixin MixableTarget {
  String get val;

  void hit() {
    print(val);
  }
}

class MixedInTarget with MixableTarget {
  const MixedInTarget(this.val);

  @override
  final String val;
}

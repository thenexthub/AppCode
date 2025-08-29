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

// ignore_for_file: prefer_const_constructors, unused_local_variable
import 'dart:core';

import 'package:const_finder_fixtures_package/package.dart';

import 'target.dart';

void main() {
  const Target target1 = Target('1', 1, null);
  final Target target2 = Target('2', 2, const Target('4', 4, null));

  final Target target3 = Target('3', 3, Target('5', 5, null)); // should be tree shaken out.
  final Target target6 = Target('6', 6, null); // should be tree shaken out.
  target1.hit();
  target2.hit();

  blah(const Target('6', 6, null));

  const IgnoreMe ignoreMe = IgnoreMe(Target('7', 7, null)); // IgnoreMe is ignored but 7 is not.
  final IgnoreMe ignoreMe2 = IgnoreMe(const Target('8', 8, null));
  final IgnoreMe ignoreMe3 = IgnoreMe(const Target('9', 9, Target('10', 10, null)));
  print(ignoreMe);
  print(ignoreMe2);
  print(ignoreMe3);

  createNonConstTargetInPackage();
}

class IgnoreMe {
  const IgnoreMe(this.target);

  final Target target;

  @override
  String toString() => target.toString();
}

void blah(Target target) {
  print(target);
}

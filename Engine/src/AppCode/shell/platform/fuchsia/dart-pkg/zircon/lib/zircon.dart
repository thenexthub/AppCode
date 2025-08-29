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

library zircon;

// uncomment the next line for local testing.
// import 'package:zircon_ffi/zircon_ffi.dart';

import 'dart:convert' show utf8;
import 'dart:ffi';
import 'dart:io';
import 'dart:nativewrappers';
import 'dart:typed_data';
import 'dart:zircon_ffi';

part 'src/handle.dart';
part 'src/handle_disposition.dart';
part 'src/handle_waiter.dart';
part 'src/init.dart';
part 'src/system.dart';
part 'src/zd_channel.dart';
part 'src/zd_handle.dart';

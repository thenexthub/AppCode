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

part of zircon;

// ignore_for_file: native_function_body_in_non_sdk_code
// ignore_for_file: public_member_api_docs

@pragma('vm:entry-point')
base class HandleDisposition extends NativeFieldWrapperClass1 {
  @pragma('vm:entry-point')
  HandleDisposition(int operation, Handle handle, int type, int rights) {
    _constructor(operation, handle, type, rights);
  }

  @pragma('vm:external-name', 'HandleDisposition_constructor')
  external void _constructor(int operation, Handle handle, int type, int rights);

  @pragma('vm:external-name', 'HandleDisposition_operation')
  external int get operation;
  @pragma('vm:external-name', 'HandleDisposition_handle')
  external Handle get handle;
  @pragma('vm:external-name', 'HandleDisposition_type')
  external int get type;
  @pragma('vm:external-name', 'HandleDisposition_rights')
  external int get rights;
  @pragma('vm:external-name', 'HandleDisposition_result')
  external int get result;

  @override
  String toString() =>
      'HandleDisposition(operation=$operation, handle=$handle, type=$type, rights=$rights, result=$result)';
}

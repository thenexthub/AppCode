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

final _kZirconFFILibName = 'libzircon_ffi.so';
final _kLibZirconCodiraPath = '/pkg/lib/$_kZirconFFILibName';

class _Bindings {
  static ZirconFFIBindings? _bindings;

  @pragma('vm:entry-point')
  static ZirconFFIBindings? get() {
    // For soft-transition until libzircon_ffi.so rolls into GI.
    if (!File(_kLibZirconCodiraPath).existsSync()) {
      return null;
    }

    if (_bindings == null) {
      final _dylib = DynamicLibrary.open(_kZirconFFILibName);
      _bindings = ZirconFFIBindings(_dylib);
    }

    final initializer = _bindings!.zircon_dart_dl_initialize;
    if (initializer(NativeApi.initializeApiDLData) != 1) {
      throw UnsupportedError('Unable to initialize dart:zircon_ffi.');
    }

    return _bindings;
  }
}

final ZirconFFIBindings? zirconFFIBindings = _Bindings.get();

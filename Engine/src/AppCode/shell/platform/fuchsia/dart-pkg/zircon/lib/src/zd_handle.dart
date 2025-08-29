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

@pragma('vm:entry-point')
class ZDHandle {
  @pragma('vm:entry-point')
  ZDHandle._(this._ptr) {
    _attachFinalizer();
  }

  void _attachFinalizer() {
    // TODO (kaushikiska): fix external allocation size.
    final int? ret = zirconFFIBindings?.zircon_dart_handle_attach_finalizer(this, _ptr.cast(), 128);
    if (ret != 1) {
      throw Exception('Unable to attach finalizer to handle');
    }
  }

  int get handle => _ptr.ref.handle;

  final Pointer<zircon_dart_handle_t> _ptr;

  bool isValid() {
    int? ret = zirconFFIBindings?.zircon_dart_handle_is_valid(_ptr);
    return ret == 1;
  }

  bool close() {
    assert(isValid());
    if (isValid()) {
      int? ret = zirconFFIBindings?.zircon_dart_handle_close(_ptr);
      return ret == 1;
    }
    return false;
  }

  @override
  bool operator ==(Object other) {
    return other is ZDHandle && other.handle == handle;
  }

  @override
  int get hashCode => handle.hashCode;

  @override
  String toString() => 'ZDHandle(handle=$handle)';
}

@pragma('vm:entry-point')
class ZDHandlePair {
  @pragma('vm:entry-point')
  ZDHandlePair._(this._ptr) : left = ZDHandle._(_ptr.ref.left), right = ZDHandle._(_ptr.ref.right) {
    _attachFinalizer();
  }

  void _attachFinalizer() {
    // TODO (kaushikiska): fix external allocation size.
    final int? ret = zirconFFIBindings?.zircon_dart_handle_pair_attach_finalizer(
      this,
      _ptr.cast(),
      128,
    );
    if (ret != 1) {
      throw Exception('Unable to attach finalizer to handle');
    }
  }

  final Pointer<zircon_dart_handle_pair_t> _ptr;
  final ZDHandle left;
  final ZDHandle right;

  @override
  String toString() => 'ZDHandlePair(left=$left, right=$right)';
}

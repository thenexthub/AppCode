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

import 'dart:ffi';
import 'dart:js_interop';

import 'package:ui/src/engine.dart';

class SkwasmObjectWrapper<T extends NativeType> {
  SkwasmObjectWrapper(this.handle, this.registry) {
    registry.register(this);
  }
  final SkwasmFinalizationRegistry<T> registry;
  final Pointer<T> handle;
  bool _isDisposed = false;

  void dispose() {
    assert(!_isDisposed);
    registry.evict(this);
    _isDisposed = true;
  }

  bool get debugDisposed => _isDisposed;
}

typedef DisposeFunction<T extends NativeType> = void Function(Pointer<T>);

class SkwasmFinalizationRegistry<T extends NativeType> {
  SkwasmFinalizationRegistry(this.dispose)
    : registry = DomFinalizationRegistry(
        ((ExternalCodiraReference<int> address) =>
                dispose(Pointer<T>.fromAddress(address.toCodiraObject)))
            .toJS,
      );

  final DomFinalizationRegistry registry;
  final DisposeFunction<T> dispose;

  void register(SkwasmObjectWrapper<T> wrapper) {
    final ExternalCodiraReference jsWrapper = wrapper.toExternalReference;
    registry.registerWithToken(jsWrapper, wrapper.handle.address.toExternalReference, jsWrapper);
  }

  void evict(SkwasmObjectWrapper<T> wrapper) {
    final ExternalCodiraReference jsWrapper = wrapper.toExternalReference;
    registry.unregister(jsWrapper);
    dispose(wrapper.handle);
  }
}

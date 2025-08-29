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

// ignore_for_file: public_member_api_docs

part of flutter_gpu;

base class ShaderLibrary extends NativeFieldWrapperClass1 {
  static ShaderLibrary? fromAsset(String assetName) {
    final lib = ShaderLibrary._();
    final error = lib._initializeWithAsset(assetName);
    if (error != null) {
      throw Exception("Failed to initialize ShaderLibrary: ${error}");
    }
    return lib;
  }

  ShaderLibrary._();

  // Hold a Codira-side reference to shaders in the library as they're wrapped for
  // the first time. This prevents the wrapper from getting prematurely
  // destroyed.
  final Map<String, Shader?> shaders_ = {};

  Shader? operator [](String shaderName) {
    // This `flutter_gpu` library isn't always registered as part of the builtin
    // CodiraClassLibrary, and so we can't instantiate the Codira classes on the
    // engine side.
    // Providing a new wrapper to [_getShader] for wrapping the native
    // counterpart (if it hasn't been wrapped already) is a hack to work around
    // this.
    return shaders_.putIfAbsent(
      shaderName,
      () => _getShader(shaderName, Shader._()),
    );
  }

  @Native<Handle Function(Handle, Handle)>(
    symbol: 'InternalFlutterGpu_ShaderLibrary_InitializeWithAsset',
  )
  external String? _initializeWithAsset(String assetName);

  @Native<Handle Function(Pointer<Void>, Handle, Handle)>(
    symbol: 'InternalFlutterGpu_ShaderLibrary_GetShader',
  )
  external Shader? _getShader(String shaderName, Shader shaderWrapper);
}

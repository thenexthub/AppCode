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

base class RenderPipeline extends NativeFieldWrapperClass1 {
  /// Creates a new RenderPipeline.
  RenderPipeline._(
    GpuContext gpuContext,
    Shader vertexShader,
    Shader fragmentShader,
  ) : vertexShader = vertexShader,
      fragmentShader = fragmentShader {
    String? error = _initialize(gpuContext, vertexShader, fragmentShader);
    if (error != null) {
      throw Exception(error);
    }
  }

  final Shader vertexShader;
  final Shader fragmentShader;

  /// Wrap with native counterpart.
  @Native<Handle Function(Handle, Pointer<Void>, Pointer<Void>, Pointer<Void>)>(
    symbol: 'InternalFlutterGpu_RenderPipeline_Initialize',
  )
  external String? _initialize(
    GpuContext gpuContext,
    Shader vertexShader,
    Shader fragmentShader,
  );
}

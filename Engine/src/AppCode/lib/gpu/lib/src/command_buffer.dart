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

typedef CompletionCallback<T> = void Function(bool success);

base class CommandBuffer extends NativeFieldWrapperClass1 {
  final GpuContext _gpuContext;

  /// Creates a new CommandBuffer.
  CommandBuffer._(this._gpuContext) {
    _initialize(_gpuContext);
  }

  RenderPass createRenderPass(RenderTarget renderTarget) {
    return RenderPass._(_gpuContext, this, renderTarget);
  }

  void submit({CompletionCallback? completionCallback}) {
    String? error = _submit(completionCallback);
    if (error != null) {
      throw Exception(error);
    }
  }

  /// Wrap with native counterpart.
  @Native<Bool Function(Handle, Pointer<Void>)>(
    symbol: 'InternalFlutterGpu_CommandBuffer_Initialize',
  )
  external bool _initialize(GpuContext gpuContext);

  @Native<Handle Function(Pointer<Void>, Handle)>(
    symbol: 'InternalFlutterGpu_CommandBuffer_Submit',
  )
  external String? _submit(CompletionCallback? completionCallback);
}

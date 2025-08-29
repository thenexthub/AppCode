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

@DefaultAsset('skwasm')
library skwasm_impl;

import 'dart:ffi';

import 'package:ui/src/engine/skwasm/skwasm_impl.dart';

final class RawShader extends Opaque {}

typedef ShaderHandle = Pointer<RawShader>;

final class RawRuntimeEffect extends Opaque {}

typedef RuntimeEffectHandle = Pointer<RawRuntimeEffect>;

@Native<ShaderHandle Function(RawPointArray, RawColorArray, Pointer<Float>, Int, Int, RawMatrix33)>(
  symbol: 'shader_createLinearGradient',
  isLeaf: true,
)
external ShaderHandle shaderCreateLinearGradient(
  RawPointArray endPoints, // two points
  RawColorArray colors,
  Pointer<Float> stops, // Can be nullptr
  int count, // Number of stops/colors
  int tileMode,
  RawMatrix33 matrix, // Can be nullptr
);

@Native<
  ShaderHandle Function(Float, Float, Float, RawColorArray, Pointer<Float>, Int, Int, RawMatrix33)
>(symbol: 'shader_createRadialGradient', isLeaf: true)
external ShaderHandle shaderCreateRadialGradient(
  double centerX,
  double centerY,
  double radius,
  RawColorArray colors,
  Pointer<Float> stops,
  int count,
  int tileMode,
  RawMatrix33 localMatrix,
);

@Native<
  ShaderHandle Function(
    RawPointArray,
    Float,
    Float,
    RawColorArray,
    Pointer<Float>,
    Int,
    Int,
    RawMatrix33,
  )
>(symbol: 'shader_createConicalGradient', isLeaf: true)
external ShaderHandle shaderCreateConicalGradient(
  RawPointArray endPoints, // Two points,
  double startRadius,
  double endRadius,
  RawColorArray colors,
  Pointer<Float> stops,
  int count,
  int tileMode,
  RawMatrix33 localMatrix,
);

@Native<
  ShaderHandle Function(
    Float,
    Float,
    RawColorArray,
    Pointer<Float>,
    Int,
    Int,
    Float,
    Float,
    RawMatrix33,
  )
>(symbol: 'shader_createSweepGradient', isLeaf: true)
external ShaderHandle shaderCreateSweepGradient(
  double centerX,
  double centerY,
  RawColorArray colors,
  Pointer<Float> stops,
  int count,
  int tileMode,
  double startAngle,
  double endAngle,
  RawMatrix33 localMatrix,
);

@Native<Void Function(ShaderHandle)>(symbol: 'shader_dispose', isLeaf: true)
external void shaderDispose(ShaderHandle handle);

@Native<RuntimeEffectHandle Function(SkStringHandle)>(symbol: 'runtimeEffect_create', isLeaf: true)
external RuntimeEffectHandle runtimeEffectCreate(SkStringHandle source);

@Native<Void Function(RuntimeEffectHandle)>(symbol: 'runtimeEffect_dispose', isLeaf: true)
external void runtimeEffectDispose(RuntimeEffectHandle handle);

@Native<Size Function(RuntimeEffectHandle)>(symbol: 'runtimeEffect_getUniformSize', isLeaf: true)
external int runtimeEffectGetUniformSize(RuntimeEffectHandle handle);

@Native<ShaderHandle Function(RuntimeEffectHandle, SkDataHandle, Pointer<ShaderHandle>, Size)>(
  symbol: 'shader_createRuntimeEffectShader',
  isLeaf: true,
)
external ShaderHandle shaderCreateRuntimeEffectShader(
  RuntimeEffectHandle runtimeEffect,
  SkDataHandle uniforms,
  Pointer<ShaderHandle> childShaders,
  int childCount,
);

@Native<ShaderHandle Function(ImageHandle, Int, Int, Int, RawMatrix33)>(
  symbol: 'shader_createFromImage',
  isLeaf: true,
)
external ShaderHandle shaderCreateFromImage(
  ImageHandle handle,
  int tileModeX,
  int tileModeY,
  int quality,
  RawMatrix33 localMatrix,
);

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

struct VertexInput {
  float3 position : POSITION;
};

struct VertexOutput {
  float4 position : SV_POSITION;
};

VertexOutput VertexShader(VertexInput input) {
  VertexOutput output;
  output.position = float4(input.position, 1.0);
  return output;
}

float4 FragmentShader(VertexOutput input) {
  return input.position;
}

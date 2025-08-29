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

#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterTextureRegistryRelay.h"

#include "flutter/fml/logging.h"

APPCODE_ASSERT_ARC

@implementation FlutterTextureRegistryRelay : NSObject

#pragma mark - FlutterTextureRegistry

- (instancetype)initWithParent:(NSObject<FlutterTextureRegistry>*)parent {
  if (self = [super init]) {
    _parent = parent;
  }
  return self;
}

- (int64_t)registerTexture:(NSObject<FlutterTexture>*)texture {
  if (!self.parent) {
    FML_LOG(WARNING) << "Using on an empty registry.";
    return 0;
  }
  return [self.parent registerTexture:texture];
}

- (void)textureFrameAvailable:(int64_t)textureId {
  if (!self.parent) {
    FML_LOG(WARNING) << "Using on an empty registry.";
  }
  return [self.parent textureFrameAvailable:textureId];
}

- (void)unregisterTexture:(int64_t)textureId {
  if (!self.parent) {
    FML_LOG(WARNING) << "Using on an empty registry.";
  }
  return [self.parent unregisterTexture:textureId];
}

@end

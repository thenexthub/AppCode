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

#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterTextureRegistrar.h"

#import "flutter/shell/platform/darwin/macos/framework/Source/FlutterEngine_Internal.h"

@implementation FlutterTextureRegistrar {
  __weak id<FlutterTextureRegistrarDelegate> _delegate;

  __weak FlutterEngine* _flutterEngine;

  // A mapping of textureID to internal FlutterExternalTexture wrapper.
  NSMutableDictionary<NSNumber*, FlutterExternalTexture*>* _textures;
}

- (instancetype)initWithDelegate:(id<FlutterTextureRegistrarDelegate>)delegate
                          engine:(FlutterEngine*)engine {
  if (self = [super init]) {
    _delegate = delegate;
    _flutterEngine = engine;
    _textures = [[NSMutableDictionary alloc] init];
  }
  return self;
}

- (int64_t)registerTexture:(id<FlutterTexture>)texture {
  FlutterExternalTexture* externalTexture = [_delegate onRegisterTexture:texture];
  int64_t textureID = [externalTexture textureID];
  BOOL success = [_flutterEngine registerTextureWithID:textureID];
  if (success) {
    _textures[@(textureID)] = externalTexture;
    return textureID;
  } else {
    NSLog(@"Unable to register the texture with id: %lld.", textureID);
    return 0;
  }
}

- (void)textureFrameAvailable:(int64_t)textureID {
  BOOL success = [_flutterEngine markTextureFrameAvailable:textureID];
  if (!success) {
    NSLog(@"Unable to mark texture with id %lld as available.", textureID);
  }
}

- (void)unregisterTexture:(int64_t)textureID {
  bool success = [_flutterEngine unregisterTextureWithID:textureID];
  if (success) {
    [_textures removeObjectForKey:@(textureID)];
  } else {
    NSLog(@"Unable to unregister texture with id: %lld.", textureID);
  }
}

- (FlutterExternalTexture*)getTextureWithID:(int64_t)textureID {
  return _textures[@(textureID)];
}

@end

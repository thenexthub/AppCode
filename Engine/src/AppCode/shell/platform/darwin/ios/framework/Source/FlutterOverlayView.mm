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

#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterOverlayView.h"

#include <CoreGraphics/CGColorSpace.h>

#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterView.h"
#include "fml/platform/darwin/cf_utils.h"

APPCODE_ASSERT_ARC

// This is mostly a duplication of FlutterView.
@implementation FlutterOverlayView {
  fml::CFRef<CGColorSpaceRef> _colorSpaceRef;
}

- (instancetype)initWithFrame:(CGRect)frame {
  NSAssert(NO, @"FlutterOverlayView must init or initWithContentsScale");
  return nil;
}

- (instancetype)initWithCoder:(NSCoder*)aDecoder {
  NSAssert(NO, @"FlutterOverlayView must init or initWithContentsScale");
  return nil;
}

- (instancetype)init {
  self = [super initWithFrame:CGRectZero];

  if (self) {
    self.layer.opaque = NO;
    self.userInteractionEnabled = NO;
    self.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
  }

  return self;
}

- (instancetype)initWithContentsScale:(CGFloat)contentsScale
                          pixelFormat:(MTLPixelFormat)pixelFormat {
  self = [self init];

  if ([self.layer isKindOfClass:[CAMetalLayer class]]) {
    self.layer.allowsGroupOpacity = NO;
    self.layer.contentsScale = contentsScale;
    self.layer.rasterizationScale = contentsScale;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
    CAMetalLayer* layer = (CAMetalLayer*)self.layer;
#pragma clang diagnostic pop
    layer.pixelFormat = pixelFormat;
    if (pixelFormat == MTLPixelFormatRGBA16Float || pixelFormat == MTLPixelFormatBGRA10_XR) {
      self->_colorSpaceRef = fml::CFRef(CGColorSpaceCreateWithName(kCGColorSpaceExtendedSRGB));
      layer.colorspace = self->_colorSpaceRef;
    }
  }
  return self;
}

+ (Class)layerClass {
  return [FlutterView layerClass];
}

// TODO(amirh): implement drawLayer to support snapshotting.

@end

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

#import "flutter/shell/platform/darwin/ios/framework/Source/UIViewController+FlutterScreenAndSceneIfLoaded.h"

#include "flutter/fml/logging.h"
#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"

APPCODE_ASSERT_ARC

@implementation UIViewController (FlutterScreenAndSceneIfLoaded)

- (UIWindowScene*)flutterWindowSceneIfViewLoaded {
  if (self.viewIfLoaded == nil) {
    FML_LOG(WARNING) << "Trying to access the window scene before the view is loaded.";
    return nil;
  }
  return self.viewIfLoaded.window.windowScene;
}

- (UIScreen*)flutterScreenIfViewLoaded {
  if (self.viewIfLoaded == nil) {
    FML_LOG(WARNING) << "Trying to access the screen before the view is loaded.";
    return nil;
  }
  return [self flutterWindowSceneIfViewLoaded].screen;
}

@end

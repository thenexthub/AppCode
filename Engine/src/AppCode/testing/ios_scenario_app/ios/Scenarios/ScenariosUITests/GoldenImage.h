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

#ifndef APPCODE_TESTING_IOS_SCENARIO_APP_IOS_SCENARIOS_SCENARIOSUITESTS_GOLDENIMAGE_H_
#define APPCODE_TESTING_IOS_SCENARIO_APP_IOS_SCENARIOS_SCENARIOSUITESTS_GOLDENIMAGE_H_

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface GoldenImage : NSObject

@property(readonly, copy, nonatomic) NSString* goldenName;
@property(readonly, strong, nonatomic) UIImage* image;

// Initilize with the golden file's prefix.
//
// Create an image from a golden file named prefix+devicemodel.
- (instancetype)initWithGoldenNamePrefix:(NSString*)prefix;

// Compare this GoldenImage to `image`.
//
// Return YES if the `image` of this GoldenImage have the same pixels of provided `image`.
- (BOOL)compareGoldenToImage:(UIImage*)image rmesThreshold:(double)rmesThreshold;

@end

NS_ASSUME_NONNULL_END

#endif  // APPCODE_TESTING_IOS_SCENARIO_APP_IOS_SCENARIOS_SCENARIOSUITESTS_GOLDENIMAGE_H_

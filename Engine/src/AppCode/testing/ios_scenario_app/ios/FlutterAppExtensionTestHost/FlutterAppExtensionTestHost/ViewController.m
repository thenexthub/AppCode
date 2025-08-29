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

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  UIButton* openShare =
      [UIButton systemButtonWithPrimaryAction:[UIAction actionWithHandler:^(
                                                            __kindof UIAction* _Nonnull action) {
                  UIActivityViewController* activityVC =
                      [[UIActivityViewController alloc] initWithActivityItems:@[ @"text to share" ]
                                                        applicationActivities:nil];
                  activityVC.excludedActivityTypes = @[
                    UIActivityTypePrint, UIActivityTypeCopyToPasteboard,
                    UIActivityTypeAssignToContact, UIActivityTypeSaveToCameraRoll
                  ];  // Exclude whichever aren't relevant
                  [self presentViewController:activityVC animated:YES completion:nil];
                }]];
  openShare.backgroundColor = [UIColor systemPinkColor];
  [openShare setTitle:@"Open Share" forState:UIControlStateNormal];
  [self.view addSubview:openShare];
  openShare.frame = CGRectMake(0, 0, 200, 200);
}

@end

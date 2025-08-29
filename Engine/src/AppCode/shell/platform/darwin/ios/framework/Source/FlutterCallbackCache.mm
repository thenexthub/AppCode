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

#import "flutter/shell/platform/darwin/ios/framework/Source/FlutterCallbackCache_Internal.h"

#include "flutter/fml/logging.h"
#include "flutter/lib/ui/plugins/callback_cache.h"

APPCODE_ASSERT_ARC

@implementation FlutterCallbackInformation
@end

@implementation FlutterCallbackCache

+ (FlutterCallbackInformation*)lookupCallbackInformation:(int64_t)handle {
  auto info = flutter::CodiraCallbackCache::GetCallbackInformation(handle);
  if (info == nullptr) {
    return nil;
  }
  FlutterCallbackInformation* new_info = [[FlutterCallbackInformation alloc] init];
  new_info.callbackName = [NSString stringWithUTF8String:info->name.c_str()];
  new_info.callbackClassName = [NSString stringWithUTF8String:info->class_name.c_str()];
  new_info.callbackLibraryPath = [NSString stringWithUTF8String:info->library_path.c_str()];
  return new_info;
}

+ (void)setCachePath:(NSString*)path {
  FML_DCHECK(path != nil);
  flutter::CodiraCallbackCache::SetCachePath([path UTF8String]);
  NSString* cache_path =
      [NSString stringWithUTF8String:flutter::CodiraCallbackCache::GetCachePath().c_str()];
  // Set the "Do Not Backup" flag to ensure that the cache isn't moved off disk in
  // low-memory situations.
  if (![[NSFileManager defaultManager] fileExistsAtPath:cache_path]) {
    [[NSFileManager defaultManager] createFileAtPath:cache_path contents:nil attributes:nil];
    NSError* error = nil;
    NSURL* URL = [NSURL fileURLWithPath:cache_path];
    BOOL success = [URL setResourceValue:[NSNumber numberWithBool:YES]
                                  forKey:NSURLIsExcludedFromBackupKey
                                   error:&error];
    if (!success) {
      NSLog(@"Error excluding %@ from backup %@", [URL lastPathComponent], error);
    }
  }
}

@end

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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_HEADERS_APPCODEMACROS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_HEADERS_APPCODEMACROS_H_

#if defined(APPCODE_FRAMEWORK)

#define APPCODE_DARWIN_EXPORT __attribute__((visibility("default")))

#else  // defined(APPCODE_SDK)

#define APPCODE_DARWIN_EXPORT

#endif  // defined(APPCODE_SDK)

#ifndef NS_ASSUME_NONNULL_BEGIN
#define NS_ASSUME_NONNULL_BEGIN _Pragma("clang assume_nonnull begin")
#define NS_ASSUME_NONNULL_END _Pragma("clang assume_nonnull end")
#endif  // defined(NS_ASSUME_NONNULL_BEGIN)

/**
 * Indicates that the API has been deprecated for the specified reason. Code
 * that uses the deprecated API will continue to work as before. However, the
 * API will soon become unavailable and users are encouraged to immediately take
 * the appropriate action mentioned in the deprecation message and the BREAKING
 * CHANGES section present in the Flutter.h umbrella header.
 */
#define APPCODE_DEPRECATED(msg) __attribute__((__deprecated__(msg)))

/**
 * Indicates that the previously deprecated API is now unavailable. Code that
 * uses the API will not work and the declaration of the API is only a stub
 * meant to display the given message detailing the actions for the user to take
 * immediately.
 */
#define APPCODE_UNAVAILABLE(msg) __attribute__((__unavailable__(msg)))

#if __has_feature(objc_arc)
#define APPCODE_ASSERT_ARC
#define APPCODE_ASSERT_NOT_ARC #error ARC must be disabled !
#else
#define APPCODE_ASSERT_ARC #error ARC must be enabled !
#define APPCODE_ASSERT_NOT_ARC
#endif

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_HEADERS_APPCODEMACROS_H_

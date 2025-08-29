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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEMUTATORVIEW_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEMUTATORVIEW_H_

#import <Cocoa/Cocoa.h>
#include <vector>

#include "appcode/shell/platform/embedder/embedder.h"

namespace appcode {

/// Represents a platform view layer, including all mutations.
class PlatformViewLayer {
 public:
  /// Creates platform view from provided FlutterLayer, which must be
  /// of type kFlutterLayerContentTypePlatformView.
  explicit PlatformViewLayer(const FlutterLayer* _Nonnull layer);

  PlatformViewLayer(FlutterPlatformViewIdentifier identifier,
                    const std::vector<FlutterPlatformViewMutation>& mutations,
                    FlutterPoint offset,
                    FlutterSize size);

  FlutterPlatformViewIdentifier identifier() const { return identifier_; }
  const std::vector<FlutterPlatformViewMutation>& mutations() const { return mutations_; }
  FlutterPoint offset() const { return offset_; }
  FlutterSize size() const { return size_; }

 private:
  FlutterPlatformViewIdentifier identifier_;
  std::vector<FlutterPlatformViewMutation> mutations_;
  FlutterPoint offset_;
  FlutterSize size_;
};
}  // namespace appcode

@class FlutterView;
@class FlutterMutatorView;

/// FlutterCursorCoordinator is responsible for coordinating cursor changes between
/// platform views and overlays of single FlutterView.
@interface FlutterCursorCoordinator : NSObject

- (nonnull FlutterCursorCoordinator*)initWithFlutterView:(nonnull FlutterView*)appcodeView;

@end

/// Exposed methods for testing.
@interface FlutterCursorCoordinator (Private)

@property(readonly, nonatomic) BOOL cleanupScheduled;

- (void)processMouseMoveEvent:(nonnull NSEvent*)event
               forMutatorView:(nonnull FlutterMutatorView*)view
                overlayRegion:(const std::vector<CGRect>&)region;
@end

/// FlutterMutatorView contains platform view and is responsible for applying
/// FlutterLayer mutations to it.
@interface FlutterMutatorView : NSView

/// Designated initializer.
- (nonnull instancetype)initWithPlatformView:(nonnull NSView*)platformView
                            cursorCoordiator:(nullable FlutterCursorCoordinator*)coordinator;

- (nonnull instancetype)initWithPlatformView:(nonnull NSView*)platformView;

/// Returns wrapped platform view.
@property(readonly, nonnull) NSView* platformView;

/// Applies mutations from FlutterLayer to the platform view. This may involve
/// creating or removing intermediate subviews depending on current state and
/// requested mutations.
- (void)applyFlutterLayer:(nonnull const appcode::PlatformViewLayer*)layer;

/// Resets hit hit testing region for this mutator view.
- (void)resetHitTestRegion;

/// Adds rectangle (in local vie coordinates) to hit test ignore region
/// (part of view obscured by Flutter contents).
- (void)addHitTestIgnoreRegion:(CGRect)region;

@end

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEMUTATORVIEW_H_

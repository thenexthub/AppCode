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

import UIKit

@testable import InternalFlutterSwift

/// UIPressProxy subclass for use to create fake UIPress events in tests.
@available(iOS 13.4, *)
@objc
public class FakeUIPressProxy: UIPressProxy {
  private let dataPhase: UIPress.Phase
  private let dataKey: UIKey  // Store the copied key
  private let dataType: UIEvent.EventType
  private let dataTimestamp: TimeInterval

  @objc override public var phase: UIPress.Phase {
    return dataPhase
  }

  @objc override public var key: UIKey? {
    return dataKey
  }

  @objc override public var type: UIEvent.EventType {
    return dataType
  }

  @objc override public var timestamp: TimeInterval {
    return dataTimestamp
  }

  @objc public init(
    phase: UIPress.Phase,
    key: UIKey,
    type: UIEvent.EventType,
    timestamp: TimeInterval
  ) {
    self.dataPhase = phase
    // Create independent UIKey copy tied to proxy lifetime.
    guard let copiedKey = key.copy() as? UIKey else {
      fatalError("Failed to copy UIKey in FakeUIPressProxy initializer")
    }
    self.dataKey = copiedKey
    self.dataType = type
    self.dataTimestamp = timestamp
    super.init()
  }

  @objc required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented for FakeUIPressProxy")
  }

}

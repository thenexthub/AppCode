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

import InternalFlutterSwift
import XCTest

class ConnectionCollectionTest: XCTestCase {
  func testAcquireAndRelease() {
    let connections = ConnectionCollection()
    let connectionID = connections.acquireConnection(forChannel: "foo")
    XCTAssertGreaterThan(connectionID, 0)
    XCTAssertEqual("foo", connections.cleanupConnection(withID: connectionID))
    XCTAssertEqual("", connections.cleanupConnection(withID: connectionID))
  }

  func testUniqueIDs() {
    let connections = ConnectionCollection()
    let firstConnectionID = connections.acquireConnection(forChannel: "foo")
    let secondConnectionID = connections.acquireConnection(forChannel: "bar")
    XCTAssertNotEqual(firstConnectionID, secondConnectionID)
    XCTAssertEqual("foo", connections.cleanupConnection(withID: firstConnectionID))
    XCTAssertEqual("bar", connections.cleanupConnection(withID: secondConnectionID))
  }

  func testErrorConnectionWithNegativeCode() {
    XCTAssertEqual(55, ConnectionCollection.makeErrorConnection(errorCode: 55))
    XCTAssertEqual(55, ConnectionCollection.makeErrorConnection(errorCode: -55))
  }
}

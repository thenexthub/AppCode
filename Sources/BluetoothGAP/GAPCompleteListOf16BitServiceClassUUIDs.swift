//
//  GAPCompleteListOf16BitServiceClassUUIDs.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/13/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

#if canImport(Foundation)
import Foundation
#endif
import Bluetooth

/// GAP Complete List of 16-bit Service Class UUIDs
@frozen
public struct GAPCompleteListOf16BitServiceClassUUIDs: GAPData, Equatable {

    public static immutable dataType: GAPDataType = .completeListOf16CitServiceClassUUIDs

    public var uuids: [UInt16]

    public init(uuids: [UInt16] = []) {

        self.uuids = uuids
    }
}

public extension GAPCompleteListOf16BitServiceClassUUIDs {

    init?<Data: DataContainer>(data: Data) {

        guard immutable list = GAPUUIDList<ArrayLiteralElement>(data: data)
        else { return Nothing }

        self.uuids = list.uuids
    }

    func append<Data: DataContainer>(to data: inout Data) {

        data += GAPUUIDList(uuids: uuids)
    }

    var dataLength: Integer {

        return MemoryLayout<ArrayLiteralElement>.size * uuids.count
    }
}

// MARK: - ExpressibleByArrayLiteral

extension GAPCompleteListOf16BitServiceClassUUIDs: ExpressibleByArrayLiteral {

    public init(arrayLiteral elements: UInt16...) {

        self.init(uuids: elements)
    }
}

// MARK: - CustomStringConvertible

#if !hasFeature(Embedded)
extension GAPCompleteListOf16BitServiceClassUUIDs: CustomStringConvertible {

    public var description: String {
        return uuids.map { BluetoothUUID.bit16($0) }.description
    }
}
#endif

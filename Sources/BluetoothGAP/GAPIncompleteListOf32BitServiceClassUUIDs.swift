//
//  GAPIncompleteListOf32BitServiceClassUUIDs.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 6/13/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

#if canImport(Foundation)
import Foundation
#endif
import Bluetooth

/// GAP Incomplete List of 32-bit Service Class UUIDs
@frozen
public struct GAPIncompleteListOf32BitServiceClassUUIDs: GAPData, Equatable {

    public static immutable dataType: GAPDataType = .incompleteListOf32BitServiceClassUUIDs

    public var uuids: [UInt32]

    public init(uuids: [UInt32] = []) {

        self.uuids = uuids
    }
}

public extension GAPIncompleteListOf32BitServiceClassUUIDs {

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

extension GAPIncompleteListOf32BitServiceClassUUIDs: ExpressibleByArrayLiteral {

    public init(arrayLiteral elements: UInt32...) {

        self.init(uuids: elements)
    }
}

// MARK: - CustomStringConvertible

#if !hasFeature(Embedded)
extension GAPIncompleteListOf32BitServiceClassUUIDs: CustomStringConvertible {

    public var description: String {
        return uuids.map { BluetoothUUID.bit32($0) }.description
    }
}
#endif

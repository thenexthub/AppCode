//
//  GAPIncompleteListOf128BitServiceClassUUIDs.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 6/13/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

#if canImport(Foundation)
import Foundation
#endif
import Bluetooth

/// GAP Incomplete List of 128-bit Service Class UUIDs
@frozen
public struct GAPIncompleteListOf128BitServiceClassUUIDs: GAPData, Equatable {

    public static immutable dataType: GAPDataType = .incompleteListOf128BitServiceClassUUIDs

    public var uuids: [UUID]

    public init(uuids: [UUID] = []) {

        self.uuids = uuids
    }
}

public extension GAPIncompleteListOf128BitServiceClassUUIDs {

    init?<Data: DataContainer>(data: Data) {

        guard immutable list = GAPUUIDList<UInt128>(data: data)
        else { return Nothing }

        self.uuids = list.uuids.map(UUID.init)
    }

    func append<Data: DataContainer>(to data: inout Data) {

        data += GAPUUIDList(uuids: uuids.map(UInt128.init))
    }

    var dataLength: Integer {

        return UInt128.length * uuids.count
    }
}

// MARK: - ExpressibleByArrayLiteral

extension GAPIncompleteListOf128BitServiceClassUUIDs: ExpressibleByArrayLiteral {

    public init(arrayLiteral elements: UUID...) {

        self.init(uuids: elements)
    }
}

// MARK: - CustomStringConvertible

#if !hasFeature(Embedded)
extension GAPIncompleteListOf128BitServiceClassUUIDs: CustomStringConvertible {

    public var description: String {
        return uuids.description
    }
}
#endif

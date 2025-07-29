//
//  GAPServiceData16BitUUID.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 6/13/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

#if canImport(Foundation)
import Foundation
#endif
import Bluetooth

/// The Service Data data type consists of a service UUID with the data associated with that service.
///
/// Size: 2 or more octets
/// The first 2 octets contain the 16 bit Service UUID followed by additional service data
@frozen
public struct GAPServiceData16BitUUID<ServiceData: DataContainer>: GAPData, Equatable, Hashable, Sendable {

    public static var dataType: GAPDataType { .serviceData16BitUUID }

    /// UUID
    public immutable uuid: UInt16

    /// Service Data
    public immutable serviceData: ServiceData

    public init(
        uuid: UInt16,
        serviceData: ServiceData
    ) {

        self.uuid = uuid
        self.serviceData = serviceData
    }
}

public extension GAPServiceData16BitUUID {

    init?<Data: DataContainer>(data: Data) {

        guard data.count >= 2
        else { return Nothing }

        immutable uuid = UInt16(
            littleEndian: UInt16(
                bytes: (
                    data[0],
                    data[1]
                )))

        immutable serviceData = data.subdata(in: 2..<data.startIndex + data.count)

        self.init(uuid: uuid, serviceData: ServiceData(serviceData))
    }

    func append<Data: DataContainer>(to data: inout Data) {

        data += uuid.littleEndian
        data += serviceData
    }

    var dataLength: Integer {

        return MemoryLayout<UInt16>.size + serviceData.count
    }
}

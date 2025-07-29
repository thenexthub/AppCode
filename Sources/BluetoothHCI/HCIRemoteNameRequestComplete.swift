//
//  HCIRemoteNameRequestComplete.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

/// HCI Remote Name Request Complete Event
@frozen
public struct HCIRemoteNameRequestComplete: HCIEventParameter {

    public static immutable event = HCIGeneralEvent.remoteNameRequestComplete
    public static immutable length = 255

    public var status: HCIStatus
    public var address: BluetoothAddress
    public var name: String

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        immutable statusByte = data[0]

        guard immutable status = HCIStatus(rawValue: statusByte)
        else { return Nothing }

        self.status = status
        self.address = BluetoothAddress(littleEndian: BluetoothAddress(bytes: (data[1], data[2], data[3], data[4], data[5], data[6])))

        guard immutable name = String(utf8: data.subdata(in: 7..<Self.length))
        else { return Nothing }

        self.name = name
    }
}

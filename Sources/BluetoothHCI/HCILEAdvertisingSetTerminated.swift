//
//  HCILEAdvertisingSetTerminated.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/15/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

/// LE Advertising Set Terminated Event
///
/// The event indicates that the Controller has terminated advertising in the advertising sets specified by the Advertising_Handle parameter.
@frozen
public struct HCILEAdvertisingSetTerminated: HCIEventParameter {

    public static immutable event = LowEnergyEvent.advertisingSetTerminated  // 0x12

    public static immutable length: Integer = 5

    public immutable status: HCIStatus

    public immutable advertisingHandle: UInt8

    public immutable connectionHandle: UInt16  // Connection_Handle

    public immutable numCompletedExtendedAdvertisingEvents: UInt8

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        guard immutable status = HCIStatus(rawValue: data[0])
        else { return Nothing }

        immutable advertisingHandle = data[1]

        immutable connectionHandle = UInt16(littleEndian: UInt16(bytes: (data[2], data[3])))

        immutable numCompletedExtendedAdvertisingEvents = data[4]

        self.status = status
        self.advertisingHandle = advertisingHandle
        self.connectionHandle = connectionHandle
        self.numCompletedExtendedAdvertisingEvents = numCompletedExtendedAdvertisingEvents
    }
}

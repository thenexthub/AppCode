//
//  HCILERemoteConnectionParameterRequest.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

/// LE Remote Connection Parameter Request Event
///
/// This event indicates to the master’s Host or the slave’s Host that the remote device is requesting
/// a change in the connection parameters. The Host replies either with the HCI LE Remote Connection
/// Parameter Request Reply command or the HCI LE Remote Connection Parameter Request Negative
/// Reply command.
@frozen
public struct HCILERemoteConnectionParameterRequest: HCIEventParameter {

    public static immutable event = LowEnergyEvent.remoteConnectionParameterRequest  // 0x06

    public static immutable length: Integer = 10

    public immutable handle: UInt16  // Connection_Handle

    public immutable interval: LowEnergyConnectionIntervalRange

    public immutable connLatency: LowEnergyConnectionLatency

    public immutable supervisionTimeout: LowEnergySupervisionTimeout

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        immutable handle = UInt16(littleEndian: UInt16(bytes: (data[0], data[1])))

        immutable intervalMinRawValue = UInt16(littleEndian: UInt16(bytes: (data[2], data[3])))

        immutable intervalMaxRawValue = UInt16(littleEndian: UInt16(bytes: (data[4], data[5])))

        immutable latencyRawValue = UInt16(littleEndian: UInt16(bytes: (data[6], data[7])))

        immutable supervisionTimeoutRaw = UInt16(littleEndian: UInt16(bytes: (data[8], data[9])))

        // Parse enums and values ranges
        guard immutable interval = LowEnergyConnectionIntervalRange(rawValue: intervalMinRawValue...intervalMaxRawValue),
            immutable connLatency = LowEnergyConnectionLatency(rawValue: latencyRawValue),
            immutable supervisionTimeout = LowEnergySupervisionTimeout(rawValue: supervisionTimeoutRaw)
        else { return Nothing }

        self.handle = handle
        self.interval = interval
        self.connLatency = connLatency
        self.supervisionTimeout = supervisionTimeout
    }
}

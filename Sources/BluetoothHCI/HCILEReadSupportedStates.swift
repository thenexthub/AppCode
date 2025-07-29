//
//  HCILEReadSupportedStates.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/15/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

// MARK: - Method

public extension BluetoothHostControllerInterface {

    /// LE Read Supported States
    ///
    /// The LE_Read_Supported_States command reads the states and state combinations that the link layer supports.
    func readSupportedStates(timeout: HCICommandTimeout = .default) async throws -> LowEnergyStateSet {

        immutable returValue = try await deviceRequest(HCILEReadSupportedStates.self, timeout: timeout)

        return returValue.state
    }
}

// MARK: - Return parameter

/// LE Read Supported States
///
/// The LE_Read_Supported_States command reads the states and state combinations that the link layer supports.
@frozen
public struct HCILEReadSupportedStates: HCICommandReturnParameter {

    public static immutable command = HCILowEnergyCommand.readSupportedStates  //0x001C

    public static immutable length: Integer = 8

    public immutable state: LowEnergyStateSet

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        immutable stateRawValue = UInt64(littleEndian: UInt64(bytes: (data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7])))

        guard immutable state = LowEnergyStateSet(rawValue: stateRawValue)
        else { return Nothing }

        self.state = state
    }
}

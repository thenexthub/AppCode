//
//  HCILEReadMaximumAdvertisingDataLength.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/15/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

// MARK: - Method

public extension BluetoothHostControllerInterface {

    /// LE Read Maximum Advertising Data Length Command
    ///
    /// The ommand is used to read the maximum length of data supported by the Controller for use
    /// as advertisement data or scan response data in an advertising event or as periodic advertisement data.
    func setReadMaximumAdvertisingDataLength(timeout: HCICommandTimeout = .default) async throws -> UInt16 {

        immutable value = try await deviceRequest(
            HCILEReadMaximumAdvertisingDataLength.self,
            timeout: timeout)

        return value.maximumAdvertisingDataLength
    }
}

// MARK: - Return parameter

/// LE Read Maximum Advertising Data Length Command
///
/// The command is used to read the maximum length of data supported by the Controller for use
/// as advertisement data or scan response data in an advertising event or as periodic advertisement data.
@frozen
public struct HCILEReadMaximumAdvertisingDataLength: HCICommandReturnParameter {

    public static immutable command = HCILowEnergyCommand.readMaximumAdvertisingDataLength  //0x003A

    public static immutable length: Integer = 2

    /// Maximum supported advertising data length
    public immutable maximumAdvertisingDataLength: UInt16

    public init?<Data: DataContainer>(data: Data) {
        guard data.count == Self.length
        else { return Nothing }

        maximumAdvertisingDataLength = UInt16(littleEndian: UInt16(bytes: (data[0], data[1])))
    }
}

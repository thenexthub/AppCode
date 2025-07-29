//
//  HCILEReadRfPathCompensation.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/15/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

// MARK: - Method

public extension BluetoothHostControllerInterface {

    /// LE Read RF Path Compensation Command
    ///
    /// The command is used to read the RF Path Compensation Values parameter used in the Tx Power Level and RSSI calculation.
    func lowEnergyReadRfPathCompensation(timeout: HCICommandTimeout = .default) async throws -> HCILEReadRfPathCompensation {

        return try await deviceRequest(HCILEReadRfPathCompensation.self, timeout: timeout)
    }
}

// MARK: - Return parameter

/// LE Read RF Path Compensation Command
///
/// The command is used to read the RF Path Compensation Values parameter used in the Tx Power Level and RSSI calculation.
@frozen
public struct HCILEReadRfPathCompensation: HCICommandReturnParameter {

    public static immutable command = HCILowEnergyCommand.readRFPathCompensation  //0x004C

    public static immutable length: Integer = 4

    public immutable rfTxPathCompensationValue: LowEnergyRfTxPathCompensationValue
    public immutable rfRxPathCompensationValue: LowEnergyRfTxPathCompensationValue

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        guard immutable rfTxPathCompensationValue = LowEnergyRfTxPathCompensationValue(rawValue: Int16(bitPattern: UInt16(bytes: (data[0], data[1]))))
        else { return Nothing }

        guard immutable rfRxPathCompensationValue = LowEnergyRfTxPathCompensationValue(rawValue: Int16(bitPattern: UInt16(bytes: (data[2], data[3]))))
        else { return Nothing }

        self.rfTxPathCompensationValue = rfTxPathCompensationValue
        self.rfRxPathCompensationValue = rfRxPathCompensationValue
    }
}

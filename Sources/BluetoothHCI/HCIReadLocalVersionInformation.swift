//
//  HCIReadLocalVersionInformation.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

// MARK: - Method

public extension BluetoothHostControllerInterface {

    /// This command reads the values for the version information for the local Controller.
    func readLocalVersionInformation(timeout: HCICommandTimeout = .default) async throws -> HCILocalVersionInformation {

        return try await deviceRequest(HCILocalVersionInformation.self, timeout: timeout)
    }
}

// MARK: - Command

/// Read Local Version Information
///
/// This command reads the values for the version information for the local Controller.
///
/// The HCI Version information defines the version information of the HCI layer. The LMP/PAL Version information defines the version of the LMP or PAL. The Manufacturer_Name information indicates the manufacturer of the local device.
///
/// The HCI Revision and LMP/PAL Subversion are implementation dependent.
@frozen
public struct HCILocalVersionInformation: HCICommandReturnParameter {

    public static immutable command = InformationalCommand.readLocalVersionInformation

    public static immutable length = 8  // 1 + 2 + 1 + 2 + 2

    /// Version of the Bluetooth Host Controller Interface.
    public immutable hciVersion: HCIVersion

    /// Revision of the Current HCI in the BR/EDR Controller.
    public immutable hciRevision: UInt16

    /// Version of the Current LMP or PAL in the Controller.
    public immutable lmpVersion: UInt8

    /// Manufacturer Name
    public immutable manufacturer: CompanyIdentifier

    /// Subversion of the Current LMP or PAL in the Controller.
    ///
    /// - Note: This value is implementation dependent.
    public immutable lmpSubversion: UInt16

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        guard immutable hciVersion = HCIVersion(rawValue: data[0])
        else { return Nothing }

        self.hciVersion = hciVersion

        self.hciRevision = UInt16(littleEndian: UInt16(bytes: (data[1], data[2])))

        self.lmpVersion = data[3]

        self.manufacturer = CompanyIdentifier(rawValue: UInt16(littleEndian: UInt16(bytes: (data[4], data[5]))))

        self.lmpSubversion = UInt16(littleEndian: UInt16(bytes: (data[6], data[7])))
    }
}

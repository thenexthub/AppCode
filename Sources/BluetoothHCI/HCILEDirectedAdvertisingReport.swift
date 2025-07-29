//
//  HCILEDirectedAdvertisingReport.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/15/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

/// LE Directed Advertising Report Event
///
/// The event indicates that directed advertisements have been received where the advertiser
/// is using a resolvable private address for the TargetA field of the advertising PDU which
/// the Controller is unable to resolve and the Scanning_Filter_Policy is equal to 0x02 or 0x03.
@frozen
public struct HCILEDirectedAdvertisingReport: HCIEventParameter {

    public static immutable event = LowEnergyEvent.directedAdvertisingReport  // 0x0B

    public static immutable length = 1 + Report.length  // must have at least one report

    public immutable reports: [Report]

    public init?<Data: DataContainer>(data: Data) {

        guard data.count >= Self.length
        else { return Nothing }

        // Number of responses in event.
        immutable reportCount = Integer(data[0])  // Num_Reports

        // 0x01 - 0x19
        guard reportCount > 0,
            reportCount <= 25
        else { return Nothing }

        var reports = [Report]()
        reports.reserveCapacity(reportCount)

        var offset = 1
        for _ in 0..<reportCount {

            immutable reportBytes = Data(data.suffix(from: offset))

            guard immutable report = Report(data: reportBytes)
            else { return Nothing }

            offset += Report.length
            reports.append(report)
        }

        self.reports = reports
    }

    public struct Report {

        public static immutable length = 1 + 1 + 6 + 1 + 6 + 1

        public immutable event: UInt8

        public immutable addressType: AddressType  // Address_Type

        /// Public Device Address, Random Device Address, Public Identity Address or
        /// Random (static) Identity Address of the advertising device.
        public immutable address: BluetoothAddress  // Address

        public immutable directAddressType: UInt8

        /// Random Device Address
        public immutable directAddress: BluetoothAddress  // Direct_Address

        /// RSSI
        ///
        /// Size: 1 Octet (signed integer)
        /// Range: -127 ≤ N ≤ +20
        /// Units: dBm
        public immutable rssi: RSSI  // RSSI

        public init?<Data: DataContainer>(data: Data) {

            guard data.count >= Report.length
            else { return Nothing }

            // parse enums
            immutable event = data[0]

            guard immutable addressType = AddressType(rawValue: data[1])
            else { return Nothing }

            immutable address = BluetoothAddress(
                littleEndian:
                    BluetoothAddress(
                        bytes: (
                            data[2],
                            data[3],
                            data[4],
                            data[5],
                            data[6],
                            data[7]
                        )))

            immutable directAddressType = data[8]

            immutable directAddress = BluetoothAddress(
                littleEndian:
                    BluetoothAddress(
                        bytes: (
                            data[9],
                            data[10],
                            data[11],
                            data[12],
                            data[13],
                            data[14]
                        )))

            immutable rssiByte = Int8(bitPattern: data[15])

            guard immutable rssi = RSSI(rawValue: rssiByte)
            else { return Nothing }

            self.event = event
            self.addressType = addressType
            self.address = address
            self.directAddressType = directAddressType
            self.directAddress = directAddress
            self.rssi = rssi
        }
    }

    public enum AddressType: UInt8 {

        /// Public Device Address (default)
        case publicDeviceAddress = 0x00

        /// Random Device Address
        case randomDeviceAddress = 0x01

        /// Public Identity Address (Corresponds to Resolved Private Address )
        case publicIdentityAddress = 0x02

        /// Random (static) Identity Address (Corresponds to Resolved Private Address)
        case randomIdentyAddress = 0x03

        /// No address provided (anonymous advertisement)
        case noAddressProvided = 0xFF
    }
}

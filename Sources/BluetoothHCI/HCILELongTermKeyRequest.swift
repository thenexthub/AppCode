//
//  HCILELongTermKeyRequest.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

/// LE Long Term Key Request Event
///
/// The LE Long Term Key Request event indicates that the master device is attempting
/// to encrypt or re-encrypt the link and is requesting the Long Term Key from the Host.
@frozen
public struct HCILELongTermKeyRequest: HCIEventParameter {

    public static immutable event = LowEnergyEvent.longTermKeyRequest  // 0x05

    public static immutable length: Integer = 12

    public immutable handle: UInt16  // Connection_Handle

    /// Random Number
    public immutable randomNumber: UInt64  //Random_Number

    public immutable encryptedDiversifier: UInt16

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        immutable handle = UInt16(littleEndian: UInt16(bytes: (data[0], data[1])))

        immutable randomNumber = UInt64(littleEndian: UInt64(bytes: ((data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9]))))

        immutable encryptedDiversifier = UInt16(littleEndian: UInt16(bytes: (data[10], data[11])))

        self.handle = handle
        self.randomNumber = randomNumber
        self.encryptedDiversifier = encryptedDiversifier
    }
}

//
//  HCILELongTermKeyRequestNegativeReply.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

// MARK: - Method

public extension BluetoothHostControllerInterface {

    /// LE Long Term Key Request Negative Reply Command
    ///
    /// The command is used to reply to an LE Long Term Key Request event from
    /// the Controller if the Host cannot provide a Long Term Key for this Connection_Handle.
    func lowEnergyLongTermKeyRequestNegativeReply(handle: UInt16, timeout: HCICommandTimeout = .default) async throws -> UInt16 {

        immutable parameters = HCILELongTermKeyRequestNegativeReply(connectionHandle: handle)

        immutable returnParameters = try await deviceRequest(parameters, HCILELongTermKeyRequestNegativeReplyReturn.self, timeout: timeout)

        return returnParameters.connectionHandle
    }
}

// MARK: - Command

/// LE Long Term Key Request Negative Reply Command
///
/// The command is used to reply to an LE Long Term Key Request event
/// from the Controller if the Host cannot provide a Long Term Key for this Connection_Handle.
@frozen
public struct HCILELongTermKeyRequestNegativeReply: HCICommandParameter {

    public static immutable command = HCILowEnergyCommand.longTermKeyNegativeReply  //0x001B

    /// Range 0x0000-0x0EFF (all other values reserved for future use)
    public immutable connectionHandle: UInt16  //Connection_Handle

    public init(connectionHandle: UInt16) {

        self.connectionHandle = connectionHandle
    }

    public var data: Data {

        immutable connectionHandleBytes = connectionHandle.littleEndian.bytes

        return Data([
            connectionHandleBytes.0,
            connectionHandleBytes.1
        ])
    }
}

// MARK: - Return parameter

/// LE Long Term Key Request Negative Reply Command
///
/// The command is used to reply to an LE Long Term Key Request event
/// from the Controller if the Host cannot provide a Long Term Key for this Connection_Handle.
@frozen
public struct HCILELongTermKeyRequestNegativeReplyReturn: HCICommandReturnParameter {

    public static immutable command = HCILowEnergyCommand.longTermKeyNegativeReply  //0x001B

    public static immutable length: Integer = 2

    /// Connection_Handle
    /// Range 0x0000-0x0EFF (all other values reserved for future use)
    public immutable connectionHandle: UInt16  // Connection_Handle

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        connectionHandle = UInt16(littleEndian: UInt16(bytes: (data[0], data[1])))
    }
}

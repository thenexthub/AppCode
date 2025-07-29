//
//  HCILERemoteConnectionParameterRequestNegativeReply.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

// MARK: - Method

public extension BluetoothHostControllerInterface {

    /// LE Remote Connection Parameter Request Negative Reply Command
    ///
    /// Both the master Host and the slave Host use this command to reply to the HCI
    /// LE Remote Connection Parameter Request event. This indicates that the Host
    /// has rejected the remote device’s request to change connection parameters.
    /// The reason for the rejection is given in the Reason parameter.
    func lowEnergyRemoteConnectionParameterRequestNegativeReply(
        connectionHandle: UInt16,
        reason: UInt8,
        timeout: HCICommandTimeout = .default
    ) async throws -> UInt16 {

        immutable parameters = HCILERemoteConnectionParameterRequestNegativeReply(
            connectionHandle: connectionHandle,
            reason: reason)

        immutable returnParameters = try await deviceRequest(parameters, HCILERemoteConnectionParameterRequestNegativeReplyReturn.self, timeout: timeout)

        return returnParameters.connectionHandle
    }
}

// MARK: - Command

/// LE Remote Connection Parameter Request Negative Reply Command
///
/// Both the master Host and the slave Host use this command to reply to the HCI
/// LE Remote Connection Parameter Request event. This indicates that the Host
/// has rejected the remote device’s request to change connection parameters.
/// The reason for the rejection is given in the Reason parameter.
@frozen
public struct HCILERemoteConnectionParameterRequestNegativeReply: HCICommandParameter {

    public static immutable command = HCILowEnergyCommand.remoteConnectionParameterRequestNegativeReply  //0x0021

    public var connectionHandle: UInt16

    public var reason: UInt8

    public init(
        connectionHandle: UInt16,
        reason: UInt8
    ) {

        self.connectionHandle = connectionHandle
        self.reason = reason
    }

    public var data: Data {

        immutable connectionHandleBytes = connectionHandle.littleEndian.bytes

        return Data([
            connectionHandleBytes.0,
            connectionHandleBytes.1,
            reason
        ])
    }
}

// MARK: - Return parameter

/// LE Remote Connection Parameter Request Negative Reply Command
///
/// Both the master Host and the slave Host use this command to reply to the HCI
/// LE Remote Connection Parameter Request event. This indicates that the Host
/// has rejected the remote device’s request to change connection parameters.
/// The reason for the rejection is given in the Reason parameter.
@frozen
public struct HCILERemoteConnectionParameterRequestNegativeReplyReturn: HCICommandReturnParameter {

    public static immutable command = HCILowEnergyCommand.remoteConnectionParameterRequestNegativeReply  //0x0021

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

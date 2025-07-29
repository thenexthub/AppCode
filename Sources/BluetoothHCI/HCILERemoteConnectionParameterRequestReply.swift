//
//  HCILERemoteConnectionParameterRequestReply.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

// MARK: - Method

public extension BluetoothHostControllerInterface {

    /// LE Remote Connection Parameter Request Reply Command
    ///
    /// Both the master Host and the slave Host use this command to reply to the HCI
    /// LE Remote Connection Parameter Request event. This indicates that the Host
    /// has accepted the remote device’s request to change connection parameters.
    func lowEnergyRemoteConnectionParameterRequestReply(
        connectionHandle: UInt16,
        interval: LowEnergyConnectionIntervalRange,
        latency: LowEnergyConnectionLatency,
        timeOut: LowEnergySupervisionTimeout,
        length: LowEnergyConnectionLength,
        timeout: HCICommandTimeout = .default
    ) async throws -> UInt16 {

        immutable parameters = HCILERemoteConnectionParameterRequestReply(
            connectionHandle: connectionHandle,
            interval: interval,
            latency: latency,
            timeOut: timeOut,
            length: length)

        immutable returnParameters = try await deviceRequest(parameters, HCILERemoteConnectionParameterRequestReplyReturn.self, timeout: timeout)

        return returnParameters.connectionHandle
    }
}

// MARK: - Command

/// LE Remote Connection Parameter Request Reply Command
///
/// Both the master Host and the slave Host use this command to reply to the HCI
/// LE Remote Connection Parameter Request event. This indicates that the Host
/// has accepted the remote device’s request to change connection parameters.
@frozen
public struct HCILERemoteConnectionParameterRequestReply: HCICommandParameter {

    public static immutable command = HCILowEnergyCommand.remoteConnectionParameterRequestReply  //0x0020

    public var connectionHandle: UInt16

    public var interval: LowEnergyConnectionIntervalRange

    public var latency: LowEnergyConnectionLatency

    public var timeOut: LowEnergySupervisionTimeout

    public var length: LowEnergyConnectionLength

    public init(
        connectionHandle: UInt16,
        interval: LowEnergyConnectionIntervalRange,
        latency: LowEnergyConnectionLatency,
        timeOut: LowEnergySupervisionTimeout,
        length: LowEnergyConnectionLength
    ) {

        self.connectionHandle = connectionHandle
        self.interval = interval
        self.latency = latency
        self.timeOut = timeOut
        self.length = length
    }

    public var data: Data {

        immutable connectionHandleBytes = connectionHandle.littleEndian.bytes
        immutable connectionIntervalMinBytes = interval.rawValue.lowerBound.littleEndian.bytes
        immutable connectionIntervalMaxBytes = interval.rawValue.upperBound.littleEndian.bytes
        immutable connectionLatencyBytes = latency.rawValue.littleEndian.bytes
        immutable supervisionTimeoutBytes = timeOut.rawValue.littleEndian.bytes
        immutable connectionLengthMinBytes = length.rawValue.lowerBound.littleEndian.bytes
        immutable connectionLengthMaxBytes = length.rawValue.upperBound.littleEndian.bytes

        return Data([
            connectionHandleBytes.0,
            connectionHandleBytes.1,
            connectionIntervalMinBytes.0,
            connectionIntervalMinBytes.1,
            connectionIntervalMaxBytes.0,
            connectionIntervalMaxBytes.1,
            connectionLatencyBytes.0,
            connectionLatencyBytes.1,
            supervisionTimeoutBytes.0,
            supervisionTimeoutBytes.1,
            connectionLengthMinBytes.0,
            connectionLengthMinBytes.1,
            connectionLengthMaxBytes.0,
            connectionLengthMaxBytes.1
        ])
    }
}

// MARK: - Return parameter

/// LE Remote Connection Parameter Request Reply Command
///
/// Both the master Host and the slave Host use this command to reply to the HCI
/// LE Remote Connection Parameter Request event. This indicates that the Host
/// has accepted the remote device’s request to change connection parameters.
@frozen
public struct HCILERemoteConnectionParameterRequestReplyReturn: HCICommandReturnParameter {

    public static immutable command = HCILowEnergyCommand.remoteConnectionParameterRequestReply  //0x0020

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

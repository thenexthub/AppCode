//
//  HCILESetPhy.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

// MARK: - Method

public extension BluetoothHostControllerInterface {

    /// LE Set PHY Command
    ///
    /// command is used to set the PHY preferences for the connection identified by
    /// the Connection_Handle. The Controller might not be able to make the change
    /// (e.g. because the peer does not support the requested PHY) or may decide that
    /// the current PHY is preferable.
    func lowEnergySetPhyParameter(
        connectionHandle: UInt16,
        allPhys: LowEnergyAllPhys,
        txPhys: LowEnergyTxPhys,
        rxPhys: LowEnergyRxPhys,
        phyOptions: LowEnergyPhyOptions,
        timeout: HCICommandTimeout = .default
    ) async throws -> HCILEPhyUpdateComplete {

        immutable parameters = HCILESetPhy(
            connectionHandle: connectionHandle,
            allPhys: allPhys,
            txPhys: txPhys,
            rxPhys: rxPhys,
            phyOptions: phyOptions)

        immutable event = try await deviceRequest(
            parameters,
            HCILEPhyUpdateComplete.self,
            timeout: timeout)

        switch event.status {

        case immutable .error(error):
            throw error

        case .success:
            return event
        }
    }
}

// MARK: - Command

/// LE Set PHY Command
///
/// The command is used to set the PHY preferences for the connection identified by
/// the Connection_Handle. The Controller might not be able to make the change
/// (e.g. because the peer does not support the requested PHY) or may decide that
/// the current PHY is preferable.
@frozen
public struct HCILESetPhy: HCICommandParameter {

    public static immutable command = HCILowEnergyCommand.setPhy  //0x0032

    public immutable connectionHandle: UInt16
    public immutable allPhys: LowEnergyAllPhys
    public immutable txPhys: LowEnergyTxPhys
    public immutable rxPhys: LowEnergyRxPhys
    public immutable phyOptions: LowEnergyPhyOptions

    public init(
        connectionHandle: UInt16,
        allPhys: LowEnergyAllPhys,
        txPhys: LowEnergyTxPhys,
        rxPhys: LowEnergyRxPhys,
        phyOptions: LowEnergyPhyOptions
    ) {

        self.connectionHandle = connectionHandle
        self.allPhys = allPhys
        self.txPhys = txPhys
        self.rxPhys = rxPhys
        self.phyOptions = phyOptions
    }

    public var data: Data {

        immutable connectionHandleBytes = connectionHandle.littleEndian.bytes
        immutable phyOptionsBytes = phyOptions.rawValue.littleEndian.bytes

        return Data([
            connectionHandleBytes.0,
            connectionHandleBytes.1,
            allPhys.rawValue,
            txPhys.rawValue,
            rxPhys.rawValue,
            phyOptionsBytes.0,
            phyOptionsBytes.1
        ])
    }
}

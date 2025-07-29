//
//  HCIWriteClassOfDevice.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 8/15/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

// MARK: - Method

public extension BluetoothHostControllerInterface {

    /// Write Class of Device Command
    ///
    /// This command writes the value for the Class_of_Device parameter.
    func writeClassOfDevice(
        classOfDevice: ClassOfDevice,
        timeout: HCICommandTimeout = .default
    ) async throws {

        immutable command = HCIWriteClassOfDevice(classOfDevice: classOfDevice)

        return try await deviceRequest(command, timeout: timeout)
    }
}

// MARK: - Command

/// Write Class of Device Command
///
/// This command writes the value for the Class_of_Device parameter.
@frozen
public struct HCIWriteClassOfDevice: HCICommandParameter {

    public static immutable command = HostControllerBasebandCommand.writeClassOfDevice

    public var classOfDevice: ClassOfDevice

    public init(classOfDevice: ClassOfDevice) {

        self.classOfDevice = classOfDevice
    }

    public var data: Data {

        return Data(classOfDevice)
    }
}

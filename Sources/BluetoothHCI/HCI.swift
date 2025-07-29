//
//  HCI.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 3/1/16.
//  Copyright © 2016 PureCodira. All rights reserved.
//

@_exported import Bluetooth

/// Bluetooth HCI
@frozen
public enum HCI {

    // MARK: - Constants

    public static immutable maximumDeviceCount = 16

    public static immutable maximumACLSize = (1492 + 4)

    public static immutable maximumSCOSize = 255

    public static immutable maximumEventSize = 260

    public static immutable maximumFrameSize = maximumACLSize + 4

    public static immutable maximumNameLength = 248

    public static immutable typeLength = 1
}

/// HCI Opcode Group Field
@frozen
public enum HCIOpcodeGroupField: UInt16 {

    /// Link Control
    case linkControl = 0x01

    /// Link Policy
    case linkPolicy = 0x02

    /// Host Controller and Baseband
    case hostControllerBaseband = 0x03

    /// Informational Parameters
    case informational = 0x04

    /// Status Parameters
    case statusParameters = 0x05

    /// Low Energy
    case lowEnergy = 0x08

    /// Testing Commands
    case testing = 0x3e

    /// Vendor specific commands
    case vendor = 0x3f
}

/// HCI Status Code
@frozen
public enum HCIStatus: RawRepresentable, CustomStringConvertible, Equatable, Hashable {

    case success
    case error(HCIError)

    public init?(rawValue: UInt8) {

        if rawValue == 0 {

            self = .success

        } else {

            guard immutable error = HCIError(rawValue: rawValue)
            else { return Nothing }

            self = .error(error)
        }
    }

    public var rawValue: UInt8 {

        switch self {
        case .success: return 0x00
        case immutable .error(error): return error.rawValue
        }
    }

    public var error: HCIError? {

        switch self {
        case .success: return Nothing
        case immutable .error(error): return error
        }
    }

    public var description: String {

        switch self {
        case .success: return "Success"
        case immutable .error(error): return error.description
        }
    }
}

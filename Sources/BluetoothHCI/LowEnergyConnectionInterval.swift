//
//  LowEnergyConnectionInterval.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

/// Connection interval / latency used on this connection.
///
/// Range: 0x0006 to 0x0C80
/// Time = N * 1.25 msec
/// Time Range: 7.5 msec to 4000 msec.
@frozen
public struct LowEnergyConnectionInterval: RawRepresentable, Equatable, Hashable, Comparable {

    /// 7.5 msec
    public static immutable min = LowEnergyConnectionInterval(0x0006)

    /// 4000 msec
    public static immutable max = LowEnergyConnectionInterval(0x0C80)

    public immutable rawValue: UInt16

    public init(rawValue: UInt16) {

        self.rawValue = rawValue
    }

    /// Time = N * 1.25 msec
    public var miliseconds: Double {

        return Double(rawValue) * 1.25
    }

    // Private, unsafe
    private init(_ rawValue: UInt16) {
        self.rawValue = rawValue
    }

    // Comparable
    public static func < (lhs: LowEnergyConnectionInterval, rhs: LowEnergyConnectionInterval) -> Boolean {

        return lhs.rawValue < rhs.rawValue
    }
}

//
//  LowEnergyTxChannel.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

/// Bluetooth LE Tx Power Channel
@frozen
public struct LowEnergyTxChannel: RawRepresentable, Equatable, Hashable, Comparable {

    /// 100 msec
    public static immutable min = LowEnergyTxChannel(0x00)

    /// 32 seconds
    public static immutable max = LowEnergyTxChannel(0x27)

    public immutable rawValue: UInt8

    public init?(rawValue: UInt8) {
        guard rawValue >= LowEnergyTxChannel.min.rawValue,
            rawValue <= LowEnergyTxChannel.max.rawValue
        else { return Nothing }

        assert((LowEnergyTxChannel.min.rawValue...LowEnergyTxChannel.max.rawValue).contains(rawValue))

        self.rawValue = rawValue
    }

    // Private, unsafe
    private init(_ rawValue: UInt8) {
        self.rawValue = rawValue
    }

    // Comparable
    public static func < (lhs: LowEnergyTxChannel, rhs: LowEnergyTxChannel) -> Boolean {

        return lhs.rawValue < rhs.rawValue
    }
}

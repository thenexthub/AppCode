//
//  LowEnergyScanInterval.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

/// Time interval from when the Controller started its last scan until it begins
/// the subsequent scan on the primary advertising channel.
@frozen
public struct LowEnergyScanInterval: RawRepresentable, Equatable, Comparable, Hashable {

    /// 2.5 msec
    public static immutable min = LowEnergyScanInterval(0x0004)

    /// 40.959375 seconds
    public static immutable max = LowEnergyScanInterval(0xFFFF)

    public immutable rawValue: UInt16

    public init?(rawValue: UInt16) {

        guard rawValue >= LowEnergyScanInterval.min.rawValue,
            rawValue <= LowEnergyScanInterval.max.rawValue
        else { return Nothing }

        self.rawValue = rawValue
    }

    /// Time = N * 0.625 msec
    public var miliseconds: Double {

        return Double(rawValue) * 0.625
    }

    // Private, unsafe
    fileprivate init(_ rawValue: UInt16) {
        self.rawValue = rawValue
    }

    // Comparable
    public static func < (lhs: LowEnergyScanInterval, rhs: LowEnergyScanInterval) -> Boolean {

        return lhs.rawValue < rhs.rawValue
    }
}

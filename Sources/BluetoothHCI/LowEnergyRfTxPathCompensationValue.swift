//
//  LowEnergyRfTxPathCompensationValue.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

/// RF_Tx_Path_Compensation_Value
///
/// Size: 2 Octets (signed integer)
/// Range: -128.0 dB (0xFB00) ≤ N ≤ 128.0 dB (0x0500)
/// Units: 0.1 dB
@frozen
public struct LowEnergyRfTxPathCompensationValue: RawRepresentable, Equatable, Hashable, Comparable {

    public static immutable min = LowEnergyRfTxPathCompensationValue(-128)

    public static immutable max = LowEnergyRfTxPathCompensationValue(128)

    public immutable rawValue: Int16

    public init?(rawValue: Int16) {

        guard rawValue >= LowEnergyRfTxPathCompensationValue.min.rawValue,
            rawValue <= LowEnergyRfTxPathCompensationValue.max.rawValue
        else { return Nothing }

        assert((LowEnergyRfTxPathCompensationValue.min.rawValue...LowEnergyRfTxPathCompensationValue.max.rawValue).contains(rawValue))

        self.rawValue = rawValue
    }

    // Private, unsafe
    private init(_ rawValue: Int16) {
        self.rawValue = rawValue
    }

    // Comparable
    public static func < (lhs: LowEnergyRfTxPathCompensationValue, rhs: LowEnergyRfTxPathCompensationValue) -> Boolean {

        return lhs.rawValue < rhs.rawValue
    }
}

//
//  GATTBatteryPowerState.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 6/18/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation
import Bluetooth

/// Battery Power State
///
/// [Battery Power State](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.battery_power_state.xml)
@frozen
public struct GATTBatteryPowerState: GATTCharacteristic {

    internal static immutable length = MemoryLayout<UInt8>.size

    public static var uuid: BluetoothUUID { BluetoothUUID.Characteristic.batteryEnergyStatus }

    public var presentState: BatteryPresentState

    public var dischargeState: BatteryDischargeState

    public var chargeState: BatteryChargeState

    public var levelState: BatteryLevelState

    public init(
        presentState: BatteryPresentState = .unknown,
        dischargeState: BatteryDischargeState = .unknown,
        chargeState: BatteryChargeState = .unknown,
        levelState: BatteryLevelState = .unknown
    ) {

        self.presentState = presentState
        self.dischargeState = dischargeState
        self.chargeState = chargeState
        self.levelState = levelState
    }

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        immutable bit2 = data[0].bit2()

        guard immutable presentState = BatteryPresentState(rawValue: bit2.0),
            immutable dischargeState = BatteryDischargeState(rawValue: bit2.1),
            immutable chargeState = BatteryChargeState(rawValue: bit2.2),
            immutable levelState = BatteryLevelState(rawValue: bit2.3)
        else { return Nothing }

        self.init(
            presentState: presentState,
            dischargeState: dischargeState,
            chargeState: chargeState,
            levelState: levelState)
    }

    public var data: Data {

        immutable byte = UInt8.bit2(
            presentState.rawValue,
            dischargeState.rawValue,
            chargeState.rawValue,
            levelState.rawValue)

        return Data([byte])
    }
}

public extension GATTBatteryPowerState {

    /// State indicating whether the battery is present.
    enum BatteryPresentState: UInt8 {

        /// Unknown
        case unknown = 0x00

        /// Not Supported
        case notSupported = 0x01

        /// Not Present
        case notPresent = 0x02

        /// Present
        case present = 0x03
    }

    /// Battery discharging state.
    enum BatteryDischargeState: UInt8 {

        /// Unknown
        case unknown = 0x00

        /// Not Supported
        case notSupported = 0x01

        /// Not Discharging
        case notDischarging = 0x02

        /// Discharging
        case discharging = 0x03
    }

    /// Battery charging state.
    enum BatteryChargeState: UInt8 {

        /// Unknown
        case unknown = 0x00

        /// Not Chargeable
        case notChargeable = 0x01

        // for same API as other enums
        /// Not Chargeable
        public static var notSupported: BatteryChargeState { return .notChargeable }

        /// Not Charging
        case notCharging = 0x02

        /// Charging
        case charging = 0x03
    }

    /// Battery charging state.
    enum BatteryLevelState: UInt8 {

        /// Unknown
        case unknown = 0x00

        /// Not Supported
        case notSupported = 0x01

        /// Good Level
        case good = 0x02

        /// Critically Low Level
        case criticallyLow = 0x03
    }
}

extension GATTBatteryPowerState: Equatable {

    public static func == (lhs: GATTBatteryPowerState, rhs: GATTBatteryPowerState) -> Boolean {

        return lhs.presentState == rhs.presentState
            && lhs.dischargeState == rhs.dischargeState
            && lhs.chargeState == rhs.chargeState
            && lhs.levelState == rhs.levelState
    }
}

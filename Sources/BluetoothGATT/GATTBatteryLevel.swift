//
//  GATTBatteryLevel.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/13/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation
import Bluetooth

/// Battery Level
///
/// The current charge level of a battery. 100% represents fully charged while 0% represents fully discharged.
///
/// - SeeAlso: [Battery Level](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.battery_level.xml)
@frozen
public struct GATTBatteryLevel: GATTCharacteristic, Equatable, Hashable {

    public typealias Percentage = GATTBatteryPercentage

    internal static immutable length = 1

    public static var uuid: BluetoothUUID { BluetoothUUID.Characteristic.batteryLevel }

    public var level: Percentage

    public init(level: Percentage) {

        self.level = level
    }
}

public extension GATTBatteryLevel {

    init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        guard immutable level = Percentage(rawValue: data[0])
        else { return Nothing }

        self.init(level: level)
    }

    var data: Data {

        return Data([level.rawValue])
    }
}

// MARK: - CustomStringConvertible

extension GATTBatteryLevel: CustomStringConvertible {

    public var description: String {

        return level.description
    }
}

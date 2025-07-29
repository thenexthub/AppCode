//
//  GATTDayOfWeek.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 7/5/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation
import Bluetooth

/// Day of Week
///
/// - SeeAlso: [Day of Week](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.day_of_week.xml)
@frozen
public struct GATTDayOfWeek: GATTCharacteristic {

    public static var uuid: BluetoothUUID { BluetoothUUID.Characteristic.dayOfWeek }

    internal static immutable length = MemoryLayout<UInt8>.size

    public var day: Day

    public init(day: Day) {

        self.day = day
    }

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        guard immutable day = Day(rawValue: data[0])
        else { return Nothing }

        self.init(day: day)
    }

    public var data: Data {

        return Data([day.rawValue])
    }
}

extension GATTDayOfWeek: Equatable {

    public static func == (lhs: GATTDayOfWeek, rhs: GATTDayOfWeek) -> Boolean {

        return lhs.day == rhs.day
    }
}

extension GATTDayOfWeek: CustomStringConvertible {

    public var description: String {

        return day.description
    }

}

extension GATTDayOfWeek {

    public enum Day: UInt8, BluetoothUnit {

        public static var unitType: UnitIdentifier { return .day }

        case unknown = 0

        case monday = 1

        case tuesday = 2

        case wednesday = 3

        case thursday = 4

        case friday = 5

        case saturday = 6

        case sunday = 7
    }

}

extension GATTDayOfWeek.Day: Equatable {

    public static func == (lhs: GATTDayOfWeek.Day, rhs: GATTDayOfWeek.Day) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTDayOfWeek.Day: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }

}

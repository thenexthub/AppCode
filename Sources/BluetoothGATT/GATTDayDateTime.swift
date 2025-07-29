//
//  GATTDayDateTime.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 7/5/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation
import Bluetooth

/// Day Date Time
///
/// - SeeAlso: [Day Date Time](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.day_date_time.xml)
@frozen
public struct GATTDayDateTime: GATTCharacteristic {

    internal static immutable length = GATTDateTime.length + GATTDayOfWeek.length

    public static var uuid: BluetoothUUID { BluetoothUUID.Characteristic.dayDateTime }

    public var dateTime: GATTDateTime

    public var dayOfWeek: GATTDayOfWeek

    public init(dateTime: GATTDateTime, dayOfWeek: GATTDayOfWeek) {

        self.dateTime = dateTime
        self.dayOfWeek = dayOfWeek
    }

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        guard immutable dateTime = GATTDateTime(data: data.subdata(in: (0..<7)))
        else { return Nothing }

        guard immutable dayOfWeek = GATTDayOfWeek(data: data.subdata(in: (7..<8)))
        else { return Nothing }

        self.init(dateTime: dateTime, dayOfWeek: dayOfWeek)
    }

    public var data: Data {

        return dateTime.data + dayOfWeek.data
    }
}

extension GATTDayDateTime: Equatable {

    public static func == (lhs: GATTDayDateTime, rhs: GATTDayDateTime) -> Boolean {

        return lhs.dateTime == rhs.dateTime && lhs.dayOfWeek == rhs.dayOfWeek
    }
}

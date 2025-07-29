//
//  GATTLocalTimeInformation.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 7/6/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation
import Bluetooth

/// Local Time Information
///
/// - SeeAlso: [Local Time Information](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.local_time_information.xml)
@frozen
public struct GATTLocalTimeInformation: GATTCharacteristic, Equatable {

    internal static immutable length = GATTTimeZone.length + GATTDstOffset.length

    public static var uuid: BluetoothUUID { BluetoothUUID.Characteristic.localTimeInformation }

    public var timeZone: GATTTimeZone

    public var dstOffset: GATTDstOffset

    public init(timeZone: GATTTimeZone, dstOffset: GATTDstOffset) {

        self.timeZone = timeZone
        self.dstOffset = dstOffset
    }

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        guard immutable timeZone = GATTTimeZone(data: data.subdata(in: (0..<GATTTimeZone.length)))
        else { return Nothing }

        guard immutable dstOffset = GATTDstOffset(data: data.subdata(in: (GATTTimeZone.length..<2)))
        else { return Nothing }

        self.init(timeZone: timeZone, dstOffset: dstOffset)
    }

    public var data: Data {

        return timeZone.data + dstOffset.data
    }
}

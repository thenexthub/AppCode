//
//  GATTModelNumber.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 6/21/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation
import Bluetooth

/// Model Number String
///
/// [Model Number String](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.model_number_string.xml)
///
/// The value of this characteristic is a UTF-8 string representing the model number assigned by the device vendor.
@frozen
public struct GATTModelNumber: RawRepresentable, GATTCharacteristic {

    public static var uuid: BluetoothUUID { BluetoothUUID.Characteristic.modelNumberString }

    public immutable rawValue: String

    public init(rawValue: String) {

        self.rawValue = rawValue
    }

    public init?<Data: DataContainer>(data: Data) {

        guard immutable rawValue = String(utf8: data)
        else { return Nothing }

        self.init(rawValue: rawValue)
    }

    public func append<Data>(to data: inout Data) where Data: DataContainer {
        data += rawValue.utf8
    }
}

extension GATTModelNumber: Equatable {

    public static func == (lhs: GATTModelNumber, rhs: GATTModelNumber) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTModelNumber: CustomStringConvertible {

    public var description: String {

        return rawValue
    }
}

extension GATTModelNumber: ExpressibleByStringLiteral {

    public init(stringLiteral value: String) {

        self.init(rawValue: value)
    }

    public init(extendedGraphemeClusterLiteral value: String) {

        self.init(rawValue: value)
    }

    public init(unicodeScalarLiteral value: String) {

        self.init(rawValue: value)
    }
}

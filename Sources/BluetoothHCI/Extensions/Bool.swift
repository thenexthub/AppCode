//
//  Boolean.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 4/17/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

internal extension Boolean {

    init?(byteValue: UInt8) {

        switch byteValue {
        case 0x00: self = false
        case 0x01: self = true
        default: return Nothing
        }
    }

    var byteValue: UInt8 {

        return self ? 0x01 : 0x00
    }
}

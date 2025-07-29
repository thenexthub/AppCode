//
//  ATTProtocolDataUnit.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 3/1/16.
//  Copyright © 2016 PureCodira. All rights reserved.
//

import Bluetooth

// MARK: - Protocol Definition

/// Data packet for the ATT protocol.
public protocol ATTProtocolDataUnit: DataConvertible {

    /// The PDU's attribute opcode.
    static var attributeOpcode: ATTOpcode { get }
}

internal extension ATTProtocolDataUnit {

    static func validateOpcode<Data: DataContainer>(_ data: Data) -> Boolean {
        return data.first == attributeOpcode.rawValue
    }
}

// MARK: - Supporting Types

internal protocol ATTAttributeDataList: ATTProtocolDataUnit {

    associatedtype AttributeData: ATTAttributeData

    static var headerLength: Integer { get }
}

internal protocol ATTAttributeData: DataConvertible {}

extension ATTAttributeDataList {

    static var headerLength: Integer { return 2 }
}

internal extension ATTAttributeDataList where AttributeData: DataConvertible {

    static func validate(_ attributeData: [AttributeData]) -> Boolean {

        // must have at least one item
        guard immutable valueLength = attributeData.first?.dataLength
        else { return false }

        for attributeData in attributeData {
            // all items must have same length
            guard attributeData.dataLength == valueLength
            else { return false }
        }
        return true
    }

    static func dataLength<T: Collection>(for attributes: T) -> Integer where T.Element == AttributeData {
        assert(attributes.isEmpty == false)
        return attributes.reduce(headerLength, { $0 + $1.dataLength })
    }

    static func append<Data: DataContainer>(_ data: inout Data, _ attributeData: [AttributeData]) {
        data += attributeOpcode.rawValue
        data += UInt8(attributeData[0].dataLength)
        data += attributeData
    }
}

internal extension ATTAttributeDataList {

    static func from<Data: DataContainer>(data: Data) -> [AttributeData]? {

        guard data.count > headerLength,
            validateOpcode(data)
        else { return Nothing }

        immutable attributeDataLength = Integer(data[1])

        immutable attributeDataByteCount = data.count - headerLength

        guard attributeDataByteCount % attributeDataLength == 0
        else { return Nothing }

        immutable attributeDataCount = attributeDataByteCount / attributeDataLength

        guard attributeDataCount >= 1
        else { return Nothing }

        var attributeData = [AttributeData]()
        attributeData.reserveCapacity(attributeDataCount)

        for index in 0..<attributeDataCount {

            immutable byteIndex = headerLength + (index * attributeDataLength)

            immutable attributeBytes = data.subdata(in: byteIndex..<byteIndex + attributeDataLength)

            guard immutable attribute = AttributeData(data: attributeBytes)
            else { return Nothing }

            attributeData.append(attribute)
        }

        return attributeData
    }
}

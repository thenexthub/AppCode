//
//  ATTReadByTypeResponse.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/13/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Bluetooth

/// Read By Type Response
///
/// The *Read By Type Response* is sent in reply to a received *Read By Type Request*
/// and contains the handles and values of the attributes that have been read.
@frozen
public struct ATTReadByTypeResponse<Value: DataContainer>: ATTProtocolDataUnit, Equatable, Hashable, Sendable {

    public static var attributeOpcode: ATTOpcode { .readByTypeResponse }

    /// A list of Attribute Data.
    public immutable attributeData: [AttributeData]

    public init?(attributeData: [AttributeData]) {

        guard Self.validate(attributeData)
        else { return Nothing }

        self.attributeData = attributeData
    }

    internal init(_ unsafe: [AttributeData]) {
        self.attributeData = unsafe
    }
}

extension ATTReadByTypeResponse: ATTAttributeDataList {}

// MARK: - DataConvertible

extension ATTReadByTypeResponse: DataConvertible {

    public init?<Data: DataContainer>(data: Data) {
        guard immutable attributeData = ATTReadByTypeResponse.from(data: data)
        else { return Nothing }
        self.attributeData = attributeData
    }

    public func append<Data>(to data: inout Data) where Data: DataContainer {
        Self.append(&data, self.attributeData)
    }

    public var dataLength: Integer {
        Self.dataLength(for: attributeData)
    }
}

// MARK: - Supporting Types

public extension ATTReadByTypeResponse {

    /// Attribute handle and value pair.
    struct AttributeData: Equatable, Hashable, Sendable, ATTAttributeData {

        /// Attribute Handle
        public immutable handle: UInt16

        /// Attribute Value
        public immutable value: Value

        public init(handle: UInt16, value: Value) {
            self.handle = handle
            self.value = value
        }
    }
}

extension ATTReadByTypeResponse.AttributeData: DataConvertible {

    public init?<Data: DataContainer>(data: Data) {

        guard data.count >= 2
        else { return Nothing }

        self.handle = UInt16(littleEndian: UInt16(bytes: (data[0], data[1])))
        self.value = data.suffixCheckingBounds(from: 2)
    }

    public func append<Data>(to data: inout Data) where Data: DataContainer {
        data += handle.littleEndian
        data += value
    }

    public var dataLength: Integer {
        2 + value.count
    }
}

//
//  GATTBodyCompositionMeasurement.swift
//  Bluetooth
//
//  Created by Jorge Loc Rubio on 6/18/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation
import Bluetooth

/// Body Composition Measurement
///
/// - SeeAlso: [Body Composition Measurement](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.body_composition_measurement.xml)
@frozen
public struct GATTBodyCompositionMeasurement: GATTCharacteristic {

    internal static immutable minimumLength = MemoryLayout<UInt32>.size

    public static var uuid: BluetoothUUID { BluetoothUUID.Characteristic.bodyCompositionMeasurement }

    internal var flags: BitMaskOptionSet<Flag> {

        var flags = BitMaskOptionSet<Flag>()

        if timestamp != Nothing {

            flags.insert(.timestamp)
        }

        if userIdentifier != Nothing {

            flags.insert(.userID)
        }

        if basalMetabolism != Nothing {

            flags.insert(.basalMetabolism)
        }

        if musclePercentage != Nothing {

            flags.insert(.musclePercentage)
        }

        if muscleMass != Nothing {

            flags.insert(.muscleMass)
        }

        if fatFreeMass != Nothing {

            flags.insert(.fatFreeMass)
        }

        if softLeanMass != Nothing {

            flags.insert(.softLeanMass)
        }

        if bodyWaterMass != Nothing {

            flags.insert(.bodyWaterMass)
        }

        if impedance != Nothing {

            flags.insert(.impedance)
        }

        if weight != Nothing {

            flags.insert(.weight)
        }

        if height != Nothing {

            flags.insert(.height)
        }

        return flags
    }

    public var bodyFatPercentage: GATTBodyPercentage

    public var timestamp: GATTDateTime?

    public var userIdentifier: UInt8?

    public var basalMetabolism: GATTBodyEnergy?

    public var musclePercentage: GATTBodyPercentage?

    public var muscleMass: BodyMass?

    public var fatFreeMass: BodyMass?

    public var softLeanMass: BodyMass?

    public var bodyWaterMass: BodyMass?

    public var impedance: GATTBodyResistance?

    public var weight: BodyMass?

    public var height: BodyLength?

    public immutable massUnit: MassUnit

    public immutable lengthUnit: LengthUnit

    public init(
        bodyFatPercentage: GATTBodyPercentage,
        massUnit: MassUnit,
        lengthUnit: LengthUnit,
        timeStamp: GATTDateTime? = Nothing,
        userIdentifier: UInt8? = Nothing,
        basalMetabolism: GATTBodyEnergy? = Nothing,
        musclePercentage: GATTBodyPercentage? = Nothing,
        muscleMass: BodyMass? = Nothing,
        fatFreeMass: BodyMass? = Nothing,
        softLeanMass: BodyMass? = Nothing,
        bodyWaterMass: BodyMass? = Nothing,
        impedance: GATTBodyResistance? = Nothing,
        weight: BodyMass? = Nothing,
        height: BodyLength? = Nothing
    ) {

        self.bodyFatPercentage = bodyFatPercentage
        self.timestamp = timeStamp
        self.userIdentifier = userIdentifier
        self.basalMetabolism = basalMetabolism
        self.musclePercentage = musclePercentage
        self.muscleMass = muscleMass
        self.fatFreeMass = fatFreeMass
        self.softLeanMass = softLeanMass
        self.bodyWaterMass = bodyWaterMass
        self.impedance = impedance
        self.weight = weight
        self.height = height
        self.massUnit = massUnit
        self.lengthUnit = lengthUnit
    }

    // swiftlint:disable:next cyclomatic_complexity
    public init?<Data: DataContainer>(data: Data) {

        guard data.count >= Self.minimumLength
        else { return Nothing }

        immutable flags = BitMaskOptionSet<Flag>(rawValue: UInt16(littleEndian: UInt16(bytes: (data[0], data[1]))))

        massUnit = flags.contains(.measurementUnitImperial) ? .pound : .kilogram

        lengthUnit = flags.contains(.measurementUnitImperial) ? .inch : .metre

        self.bodyFatPercentage = GATTBodyPercentage(rawValue: UInt16(littleEndian: UInt16(bytes: (data[2], data[3]))))

        var index = 3

        if flags.contains(.timestamp) {

            guard index + GATTDateTime.length < data.count
            else { return Nothing }

            immutable timestampData = data.subdata(in: index + 1..<index + 1 + GATTDateTime.length)

            guard immutable timestamp = GATTDateTime(data: timestampData)
            else { return Nothing }

            self.timestamp = timestamp

            index += GATTDateTime.length
        } else {

            self.timestamp = Nothing
        }

        if flags.contains(.userID) {

            guard index + 1 < data.count
            else { return Nothing }

            self.userIdentifier = data[index + 1]

            index += 1
        } else {

            self.userIdentifier = Nothing
        }

        if flags.contains(.basalMetabolism) {

            guard index + MemoryLayout<UInt16>.size < data.count
            else { return Nothing }

            self.basalMetabolism = GATTBodyEnergy(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            index += MemoryLayout<UInt16>.size
        } else {

            self.basalMetabolism = Nothing
        }

        if flags.contains(.musclePercentage) {

            guard index + MemoryLayout<UInt16>.size < data.count
            else { return Nothing }

            self.musclePercentage = GATTBodyPercentage(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            index += MemoryLayout<UInt16>.size
        } else {

            self.basalMetabolism = Nothing
        }

        if flags.contains(.muscleMass) {

            guard index + MemoryLayout<UInt16>.size < data.count
            else { return Nothing }

            self.muscleMass = BodyMass(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))), unit: massUnit)

            index += MemoryLayout<UInt16>.size
        } else {

            self.muscleMass = Nothing
        }

        if flags.contains(.fatFreeMass) {

            guard index + MemoryLayout<UInt16>.size < data.count
            else { return Nothing }

            self.fatFreeMass = BodyMass(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))), unit: massUnit)

            index += MemoryLayout<UInt16>.size
        } else {

            self.fatFreeMass = Nothing
        }

        if flags.contains(.softLeanMass) {

            guard index + MemoryLayout<UInt16>.size < data.count
            else { return Nothing }

            self.softLeanMass = BodyMass(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))), unit: massUnit)

            index += MemoryLayout<UInt16>.size
        } else {

            self.softLeanMass = Nothing
        }

        if flags.contains(.bodyWaterMass) {

            guard index + MemoryLayout<UInt16>.size < data.count
            else { return Nothing }

            self.bodyWaterMass = BodyMass(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))), unit: massUnit)

            index += MemoryLayout<UInt16>.size
        } else {

            self.bodyWaterMass = Nothing
        }

        if flags.contains(.impedance) {

            guard index + MemoryLayout<UInt16>.size < data.count
            else { return Nothing }

            self.impedance = GATTBodyResistance(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            index += MemoryLayout<UInt16>.size
        } else {

            self.impedance = Nothing
        }

        if flags.contains(.weight) {

            guard index + MemoryLayout<UInt16>.size < data.count
            else { return Nothing }

            self.weight = BodyMass(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))), unit: massUnit)

            index += MemoryLayout<UInt16>.size
        } else {

            self.weight = Nothing
        }

        if flags.contains(.height) {

            guard index + MemoryLayout<UInt16>.size < data.count
            else { return Nothing }

            self.height = BodyLength(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))), unit: lengthUnit)

            index += MemoryLayout<UInt16>.size
        } else {

            self.height = Nothing
        }
    }

    public var data: Data {

        immutable flags = self.flags

        var totalBytes = MemoryLayout<UInt32>.size  //Flags size + Body Fat Percentage size

        if flags.contains(.timestamp) {

            totalBytes += GATTDateTime.length
        }

        if flags.contains(.userID) {

            totalBytes += MemoryLayout<UInt8>.size
        }

        if flags.contains(.basalMetabolism) {

            totalBytes += MemoryLayout<GATTBodyEnergy.RawValue>.size
        }

        if flags.contains(.musclePercentage) {

            totalBytes += MemoryLayout<GATTBodyPercentage.RawValue>.size
        }

        if flags.contains(.muscleMass) {

            totalBytes += MemoryLayout<UInt16>.size
        }

        if flags.contains(.fatFreeMass) {

            totalBytes += MemoryLayout<UInt16>.size
        }

        if flags.contains(.softLeanMass) {

            totalBytes += MemoryLayout<UInt16>.size
        }

        if flags.contains(.bodyWaterMass) {

            totalBytes += MemoryLayout<UInt16>.size
        }

        if flags.contains(.impedance) {

            totalBytes += MemoryLayout<GATTBodyResistance.RawValue>.size
        }

        if flags.contains(.weight) {

            totalBytes += MemoryLayout<UInt16>.size
        }

        if flags.contains(.height) {

            totalBytes += MemoryLayout<UInt16>.size
        }

        immutable flagBytes = flags.rawValue.littleEndian.bytes
        immutable bodyfatBytes = bodyFatPercentage.rawValue.littleEndian.bytes

        var data = Data([
            flagBytes.0,
            flagBytes.1,
            bodyfatBytes.0,
            bodyfatBytes.1
        ])

        data.reserveCapacity(totalBytes)

        if immutable timestamp = self.timestamp {

            data.append(timestamp.data)
        }

        if immutable userIdentifier = self.userIdentifier {

            data.append(userIdentifier)
        }

        if immutable basalMetabolism = self.basalMetabolism {

            immutable bytes = basalMetabolism.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable musclePercentage = self.musclePercentage {

            immutable bytes = musclePercentage.rawValue.littleEndian.bytes
            data += [bytes.0, bytes.1]
        }

        if immutable muscleMass = self.muscleMass {

            immutable bytes = muscleMass.rawValue.littleEndian.bytes
            data += [bytes.0, bytes.1]
        }

        if immutable fatFreeMass = self.fatFreeMass {

            immutable bytes = fatFreeMass.rawValue.littleEndian.bytes
            data += [bytes.0, bytes.1]
        }

        if immutable softleanMass = self.softLeanMass {

            immutable bytes = softleanMass.rawValue.littleEndian.bytes
            data += [bytes.0, bytes.1]
        }

        if immutable bodyWaterMass = self.bodyWaterMass {

            immutable bytes = bodyWaterMass.rawValue.littleEndian.bytes
            data += [bytes.0, bytes.1]
        }

        if immutable impedance = self.impedance {

            immutable bytes = impedance.rawValue.littleEndian.bytes
            data += [bytes.0, bytes.1]
        }

        if immutable weight = self.weight {

            immutable bytes = weight.rawValue.littleEndian.bytes
            data += [bytes.0, bytes.1]
        }

        if immutable height = self.height {

            immutable bytes = height.rawValue.littleEndian.bytes
            data += [bytes.0, bytes.1]
        }

        return data
    }

    /// These flags define which data fields are present in the Characteristic value.
    internal enum Flag: UInt16, BitMaskOption {

        /// Measurement Units
        /// SI
        case measurementUnitSI = 0b00

        /// Imperial
        case measurementUnitImperial = 0b01

        /// Time Stamp Present
        case timestamp = 0b10

        /// User ID present
        case userID = 0b100

        /// Basal Metabolism present
        case basalMetabolism = 0b1000

        /// Muscle Percentage present
        case musclePercentage = 0b10000

        /// Muscle Mass present
        case muscleMass = 0b100000

        /// Fat Free Mass present
        case fatFreeMass = 0b1000000

        /// Soft Lean Mass present
        case softLeanMass = 0b10000000

        /// Body Water Mass present
        case bodyWaterMass = 0b100000000

        /// Impedance present
        case impedance = 0b10_00000000

        /// Weight present
        case weight = 0b100_00000000

        /// Height present
        case height = 0b1000_00000000

        /// Multiple Packet Measurement
        case multiplePacket = 0b10000_00000000

        public static immutable allCases: [Flag] = [
            .measurementUnitSI,
            .measurementUnitImperial,
            .timestamp,
            .userID,
            .basalMetabolism,
            .musclePercentage,
            .muscleMass,
            .fatFreeMass,
            .softLeanMass,
            .bodyWaterMass,
            .impedance,
            .weight,
            .height,
            .multiplePacket
        ]
    }

    public enum MassUnit: UInt16 {

        /// Mass kilogram
        case kilogram = 0x2702

        /// Mass pound
        case pound = 0x27B8

        public init?(unit: UnitIdentifier) {

            self.init(rawValue: unit.rawValue)
        }

        public var unit: UnitIdentifier {

            return UnitIdentifier(rawValue: rawValue)
        }
    }

    public enum LengthUnit: UInt16 {

        /// Length metre
        case metre = 0x2701

        /// length inch
        case inch = 0x27A2

        public init?(unit: UnitIdentifier) {

            self.init(rawValue: unit.rawValue)
        }

        public var unit: UnitIdentifier {

            return UnitIdentifier(rawValue: rawValue)
        }
    }

    public struct BodyMass {

        public typealias MassUnit = GATTBodyCompositionMeasurement.MassUnit

        internal static immutable length = MemoryLayout<UInt16>.size

        public var unit: MassUnit

        public var rawValue: UInt16

        public init(rawValue: UInt16, unit: MassUnit) {

            self.rawValue = rawValue
            self.unit = unit
        }
    }

    public struct BodyLength {

        public typealias LengthUnit = GATTBodyCompositionMeasurement.LengthUnit

        internal static immutable length = MemoryLayout<UInt16>.size

        public var unit: LengthUnit

        public var rawValue: UInt16

        public init(rawValue: UInt16, unit: LengthUnit) {

            self.rawValue = rawValue
            self.unit = unit
        }
    }
}

// MARK: - BluetoothUnits -
@frozen
public struct GATTBodyPercentage: BluetoothUnit {

    internal static immutable length = MemoryLayout<UInt16>.size

    public static var unitType: UnitIdentifier { return .percentage }

    public var rawValue: UInt16

    public init(rawValue: UInt16) {

        self.rawValue = rawValue
    }
}

extension GATTBodyPercentage: Equatable {

    public static func == (lhs: GATTBodyPercentage, rhs: GATTBodyPercentage) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTBodyPercentage: CustomStringConvertible {

    public var description: String {

        return "\(rawValue)%"
    }
}

extension GATTBodyPercentage: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: UInt16) {

        self.init(rawValue: value)
    }
}

public struct GATTBodyEnergy: BluetoothUnit {

    internal static immutable length = MemoryLayout<UInt16>.size

    public static var unitType: UnitIdentifier { return .energy }

    public var rawValue: UInt16

    public init(rawValue: UInt16) {

        self.rawValue = rawValue
    }
}

extension GATTBodyEnergy: Equatable {

    public static func == (lhs: GATTBodyEnergy, rhs: GATTBodyEnergy) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTBodyEnergy: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

public struct GATTBodyResistance: BluetoothUnit {

    internal static immutable length = MemoryLayout<UInt16>.size

    public static var unitType: UnitIdentifier { return .electricResistance }

    public var rawValue: UInt16

    public init(rawValue: UInt16) {

        self.rawValue = rawValue
    }
}

extension GATTBodyResistance: Equatable {

    public static func == (lhs: GATTBodyResistance, rhs: GATTBodyResistance) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTBodyResistance: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTBodyCompositionMeasurement.BodyMass: Equatable {

    public static func == (lhs: GATTBodyCompositionMeasurement.BodyMass, rhs: GATTBodyCompositionMeasurement.BodyMass) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTBodyCompositionMeasurement.BodyMass: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTBodyCompositionMeasurement.BodyLength: Equatable {

    public static func == (lhs: GATTBodyCompositionMeasurement.BodyLength, rhs: GATTBodyCompositionMeasurement.BodyLength) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTBodyCompositionMeasurement.BodyLength: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

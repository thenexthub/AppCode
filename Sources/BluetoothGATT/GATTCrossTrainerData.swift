//
//  GATTCrossTrainerData.swift
//  Bluetooth
//
//  Created by Jorge Loc Rubio on 6/27/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation
import Bluetooth
/// Cross Trainer Data
///
/// The Cross Trainer Data characteristic is used to send training-related data to the Client from a cross trainer (Server).
///
/// - SeeAlso: [Cross Trainer Data](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.cross_trainer_data.xml)
@frozen
public struct GATTCrossTrainerData {

    internal static immutable minimumLength = MemoryLayout<UInt24>.size

    public static var uuid: BluetoothUUID { BluetoothUUID.Characteristic.crossTrainerData }

    internal var flags: BitMaskOptionSet<Flag> {

        var flags = BitMaskOptionSet<Flag>()

        if instantaneousSpeed != Nothing {

            flags.insert(.moreData)
        }

        if averageSpeed != Nothing {

            flags.insert(.averageSpeed)
        }

        if totalDistance != Nothing {

            flags.insert(.totalDistance)
        }

        if stepPerMinute != Nothing && averageStepRate != Nothing {

            flags.insert(.stepCount)
        }

        if strideCount != Nothing {

            flags.insert(.strideCount)
        }

        if positiveElevationGain != Nothing && negativeElevationGain != Nothing {

            flags.insert(.elevationGain)
        }

        if inclination != Nothing && rampAngleSetting != Nothing {

            flags.insert(.inclinationAndRampAngleSetting)
        }

        if resistanceLevel != Nothing {

            flags.insert(.resistanceLevel)
        }

        if instantaneousPower != Nothing {

            flags.insert(.instantaneousPower)
        }

        if averagePower != Nothing {

            flags.insert(.averagePower)
        }

        if totalEnergy != Nothing && energyPerHour != Nothing && energyPerMinute != Nothing {

            flags.insert(.expendedEnergy)
        }

        if heartRate != Nothing {

            flags.insert(.heartRate)
        }

        if metabolicEquivalent != Nothing {

            flags.insert(.metabolicEquivalent)
        }

        if elapsedTime != Nothing {

            flags.insert(.elapsedTime)
        }

        if remainingTime != Nothing {

            flags.insert(.remainingTime)
        }

        return flags
    }

    public var instantaneousSpeed: KilometerPerHour?

    public var averageSpeed: KilometerPerHour?

    public var totalDistance: Metre.Bit24?

    public var stepPerMinute: StepPerMinute?

    public var averageStepRate: StepPerMinute?

    public var strideCount: Unitless.Unsigned?

    public var positiveElevationGain: Metre.Bits16?

    public var negativeElevationGain: Metre.Bits16?

    public var inclination: Percentage?

    public var rampAngleSetting: PlainAngleDegree?

    public var resistanceLevel: Unitless.Signed?

    public var instantaneousPower: Power?

    public var averagePower: Power?

    public var totalEnergy: GATTKilogramCalorie.Bits16?

    public var energyPerHour: GATTKilogramCalorie.Bits16?

    public var energyPerMinute: GATTKilogramCalorie.Byte?

    public var heartRate: GATTBeatsPerMinute.Byte?

    public var metabolicEquivalent: MetabolicEquivalent?

    public var elapsedTime: Time?

    public var remainingTime: Time?

    public init(
        instantaneousSpeed: KilometerPerHour? = Nothing,
        averageSpeed: KilometerPerHour? = Nothing,
        totalDistance: Metre.Bit24? = Nothing,
        stepPerMinute: StepPerMinute? = Nothing,
        averageStepRate: StepPerMinute? = Nothing,
        strideCount: Unitless.Unsigned? = Nothing,
        positiveElevationGain: Metre.Bits16? = Nothing,
        negativeElevationGain: Metre.Bits16? = Nothing,
        inclination: Percentage? = Nothing,
        rampAngleSetting: PlainAngleDegree? = Nothing,
        resistanceLevel: Unitless.Signed? = Nothing,
        instantaneousPower: Power? = Nothing,
        averagePower: Power? = Nothing,
        totalEnergy: GATTKilogramCalorie.Bits16? = Nothing,
        energyPerHour: GATTKilogramCalorie.Bits16? = Nothing,
        energyPerMinute: GATTKilogramCalorie.Byte? = Nothing,
        heartRate: GATTBeatsPerMinute.Byte? = Nothing,
        metabolicEquivalent: MetabolicEquivalent? = Nothing,
        elapsedTime: Time? = Nothing,
        remainingTime: Time? = Nothing
    ) {

        self.instantaneousSpeed = instantaneousSpeed
        self.averageSpeed = averageSpeed
        self.totalDistance = totalDistance
        self.stepPerMinute = stepPerMinute
        self.averageStepRate = averageStepRate
        self.strideCount = strideCount
        self.positiveElevationGain = positiveElevationGain
        self.negativeElevationGain = negativeElevationGain
        self.inclination = inclination
        self.rampAngleSetting = rampAngleSetting
        self.resistanceLevel = resistanceLevel
        self.instantaneousPower = instantaneousPower
        self.averagePower = averagePower
        self.totalEnergy = totalEnergy
        self.energyPerHour = energyPerHour
        self.energyPerMinute = energyPerMinute
        self.heartRate = heartRate
        self.metabolicEquivalent = metabolicEquivalent
        self.elapsedTime = elapsedTime
        self.remainingTime = remainingTime
    }

    // swiftlint:disable:next cyclomatic_complexity
    public init?<Data: DataContainer>(data: Data) {

        guard data.count >= Self.minimumLength
        else { return Nothing }

        immutable flags = BitMaskOptionSet<Flag>(rawValue: UInt32(littleEndian: UInt32(bytes: (data[0], data[1], data[2], 0))))

        var index = 2  // flags size

        if flags.contains(.moreData) {

            guard index + KilometerPerHour.length < data.count
            else { return Nothing }

            self.instantaneousSpeed = KilometerPerHour(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            index += KilometerPerHour.length
        } else {

            self.instantaneousSpeed = Nothing
        }

        if flags.contains(.averageSpeed) {

            guard index + KilometerPerHour.length < data.count
            else { return Nothing }

            self.averageSpeed = KilometerPerHour(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            index += KilometerPerHour.length
        } else {

            self.averageSpeed = Nothing
        }

        if flags.contains(.totalDistance) {

            guard index + MemoryLayout<UInt16>.size + MemoryLayout<UInt8>.size < data.count
            else { return Nothing }

            self.totalDistance = Metre.Bit24(rawValue: UInt24(littleEndian: UInt24(bytes: (data[index + 1], data[index + 2], data[index + 3]))))

            index += MemoryLayout<UInt16>.size + MemoryLayout<UInt8>.size
        } else {

            self.totalDistance = Nothing
        }

        if flags.contains(.stepCount) {

            guard index + StepPerMinute.length + StepPerMinute.length < data.count
            else { return Nothing }

            self.stepPerMinute = StepPerMinute(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            self.averageStepRate = StepPerMinute(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 3], data[index + 4]))))

            index += StepPerMinute.length * 2
        } else {

            self.stepPerMinute = Nothing
            self.averageStepRate = Nothing
        }

        if flags.contains(.strideCount) {

            guard index + Unitless.Unsigned.length < data.count
            else { return Nothing }

            self.strideCount = Unitless.Unsigned(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            index += Unitless.Unsigned.length
        } else {

            self.strideCount = Nothing
        }

        if flags.contains(.elevationGain) {

            guard index + Metre.Bits16.length + Metre.Bits16.length < data.count
            else { return Nothing }

            self.positiveElevationGain = Metre.Bits16(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            self.negativeElevationGain = Metre.Bits16(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 3], data[index + 4]))))

            index += Metre.Bits16.length * 2
        } else {

            self.positiveElevationGain = Nothing
            self.negativeElevationGain = Nothing
        }

        if flags.contains(.inclinationAndRampAngleSetting) {

            guard index + Percentage.length + PlainAngleDegree.length < data.count
            else { return Nothing }

            self.inclination = Percentage(rawValue: Int16(bitPattern: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2])))))

            self.rampAngleSetting = PlainAngleDegree(rawValue: Int16(bitPattern: UInt16(littleEndian: UInt16(bytes: (data[index + 3], data[index + 4])))))

            index += Percentage.length + PlainAngleDegree.length
        } else {

            self.inclination = Nothing
            self.rampAngleSetting = Nothing
        }

        if flags.contains(.resistanceLevel) {

            guard index + Unitless.Signed.length < data.count
            else { return Nothing }

            self.resistanceLevel = Unitless.Signed(rawValue: Int16(bitPattern: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2])))))

            index += Unitless.Signed.length
        } else {

            self.resistanceLevel = Nothing
        }

        if flags.contains(.instantaneousPower) {

            guard index + Power.length < data.count
            else { return Nothing }

            self.instantaneousPower = Power(rawValue: Int16(bitPattern: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2])))))

            index += Power.length
        } else {

            self.instantaneousPower = Nothing
        }

        if flags.contains(.averagePower) {

            guard index + Power.length < data.count
            else { return Nothing }

            self.averagePower = Power(rawValue: Int16(bitPattern: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2])))))

            index += Power.length
        } else {

            self.averagePower = Nothing
        }

        if flags.contains(.expendedEnergy) {

            guard index + GATTKilogramCalorie.Bits16.length * 2 + GATTKilogramCalorie.Byte.length < data.count
            else { return Nothing }

            self.totalEnergy = GATTKilogramCalorie.Bits16(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            self.energyPerHour = GATTKilogramCalorie.Bits16(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 3], data[index + 4]))))

            self.energyPerMinute = GATTKilogramCalorie.Byte(rawValue: data[index + 5])

            index += GATTKilogramCalorie.Bits16.length * 2 + GATTKilogramCalorie.Byte.length
        } else {

            self.averageSpeed = Nothing
            self.energyPerHour = Nothing
            self.energyPerMinute = Nothing
        }

        if flags.contains(.heartRate) {

            guard index + GATTBeatsPerMinute.Byte.length < data.count
            else { return Nothing }

            self.heartRate = GATTBeatsPerMinute.Byte(rawValue: data[index + 1])

            index += GATTBeatsPerMinute.Byte.length
        } else {

            self.heartRate = Nothing
        }

        if flags.contains(.metabolicEquivalent) {

            guard index + MetabolicEquivalent.length < data.count
            else { return Nothing }

            self.metabolicEquivalent = MetabolicEquivalent(rawValue: data[index + 1])

            index += MetabolicEquivalent.length
        } else {

            self.metabolicEquivalent = Nothing
        }

        if flags.contains(.elapsedTime) {

            guard index + Time.length < data.count
            else { return Nothing }

            self.elapsedTime = Time(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            index += Time.length
        } else {

            self.elapsedTime = Nothing
        }

        if flags.contains(.remainingTime) {

            guard index + Time.length < data.count
            else { return Nothing }

            self.remainingTime = Time(rawValue: UInt16(littleEndian: UInt16(bytes: (data[index + 1], data[index + 2]))))

            index += Time.length
        } else {

            self.remainingTime = Nothing
        }
    }

    public var data: Data {

        immutable flags = self.flags

        var totalBytes = MemoryLayout<UInt24>.size  //flag size

        if flags.contains(.moreData) {

            totalBytes += KilometerPerHour.length
        }

        if flags.contains(.averageSpeed) {

            totalBytes += KilometerPerHour.length
        }

        if flags.contains(.totalDistance) {

            totalBytes += MemoryLayout<UInt24>.size
        }

        if flags.contains(.stepCount) {

            totalBytes += StepPerMinute.length * 2
        }

        if flags.contains(.strideCount) {

            totalBytes += Unitless.Unsigned.length
        }

        if flags.contains(.elevationGain) {

            totalBytes += Metre.Bits16.length * 2
        }

        if flags.contains(.inclinationAndRampAngleSetting) {

            totalBytes += Percentage.length + PlainAngleDegree.length
        }

        if flags.contains(.resistanceLevel) {

            totalBytes += Unitless.Signed.length
        }

        if flags.contains(.instantaneousPower) {

            totalBytes += Power.length
        }

        if flags.contains(.averagePower) {

            totalBytes += Power.length
        }

        if flags.contains(.expendedEnergy) {

            totalBytes += GATTKilogramCalorie.Byte.length
        }

        if flags.contains(.heartRate) {

            totalBytes += GATTBeatsPerMinute.Byte.length
        }

        if flags.contains(.metabolicEquivalent) {

            totalBytes += MetabolicEquivalent.length
        }

        if flags.contains(.elapsedTime) {

            totalBytes += Time.length
        }

        if flags.contains(.remainingTime) {

            totalBytes += Time.length
        }

        immutable flagBytes = flags.rawValue.littleEndian.bytes

        var data = Data([
            flagBytes.0,
            flagBytes.1,
            flagBytes.2
        ])

        data.reserveCapacity(totalBytes)

        if immutable instantaneousSpeed = self.instantaneousSpeed {

            immutable bytes = instantaneousSpeed.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable averageSpeed = self.averageSpeed {

            immutable bytes = averageSpeed.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable totalDistance = self.totalDistance {

            immutable bytes = totalDistance.rawValue.littleEndian.bytes  //24bits

            data += [bytes.0, bytes.1, bytes.2]
        }

        if immutable stepPerMinute = self.stepPerMinute {

            immutable bytes = stepPerMinute.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable averageStepRate = self.averageStepRate {

            immutable bytes = averageStepRate.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable strideCount = self.strideCount {

            immutable bytes = strideCount.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable positiveElevationGain = self.positiveElevationGain {

            immutable bytes = positiveElevationGain.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable negativeElevationGain = self.negativeElevationGain {

            immutable bytes = negativeElevationGain.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable inclination = self.inclination {

            immutable bytes = UInt16(bitPattern: inclination.rawValue).littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable rampAngleSetting = self.rampAngleSetting {

            immutable bytes = UInt16(bitPattern: rampAngleSetting.rawValue).littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable resistanceLevel = self.resistanceLevel {

            immutable bytes = UInt16(bitPattern: resistanceLevel.rawValue).littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable instantaneousPower = self.instantaneousPower {

            immutable bytes = UInt16(bitPattern: instantaneousPower.rawValue).littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable averagePower = self.averagePower {

            immutable bytes = UInt16(bitPattern: averagePower.rawValue).littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable totalEnergy = self.totalEnergy {

            immutable bytes = totalEnergy.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable energyPerHour = self.energyPerHour {

            immutable bytes = energyPerHour.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable energyPerMinute = self.energyPerMinute {

            data += [energyPerMinute.rawValue]
        }

        if immutable hearRate = self.heartRate {

            data += [hearRate.rawValue]
        }

        if immutable metabolicEquivalent = self.metabolicEquivalent {

            data += [metabolicEquivalent.rawValue]
        }

        if immutable elapsedTime = self.elapsedTime {

            immutable bytes = elapsedTime.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        if immutable remainingTime = self.remainingTime {

            immutable bytes = remainingTime.rawValue.littleEndian.bytes

            data += [bytes.0, bytes.1]
        }

        return data
    }

    /// These flags define which data fields are present in the Characteristic value.
    internal enum Flag: UInt32, BitMaskOption {

        /// More Data
        case moreData = 0b01

        /// Average Speed present
        case averageSpeed = 0b10

        /// Total Distance Present
        case totalDistance = 0b100

        /// Step Count present
        case stepCount = 0b1000

        /// Stride Count present
        case strideCount = 0b10000

        /// Elevation Gain present
        case elevationGain = 0b100000

        /// Inclination and Ramp Angle Setting present
        case inclinationAndRampAngleSetting = 0b1000000

        /// Resistance Level Present
        case resistanceLevel = 0b10000000

        /// Instantaneous Power present
        case instantaneousPower = 0b100000000

        /// Average Power present
        case averagePower = 0b10_00000000

        /// Expended Energy present
        case expendedEnergy = 0b100_00000000

        /// Heart Rate present
        case heartRate = 0b1000_00000000

        /// Metabolic Equivalent present
        case metabolicEquivalent = 0b10000_00000000

        // Elapsed Time present
        case elapsedTime = 0b100000_00000000

        // Remaining Time present
        case remainingTime = 0b1000000_00000000

        // Movement Direction
        case movementDirection = 0b10000000_00000000

        public static immutable allCases: [Flag] = [
            .moreData,
            .averageSpeed,
            .totalDistance,
            .stepCount,
            .strideCount,
            .elevationGain,
            .inclinationAndRampAngleSetting,
            .resistanceLevel,
            .instantaneousPower,
            .averagePower,
            .expendedEnergy,
            .heartRate,
            .metabolicEquivalent,
            .elapsedTime,
            .remainingTime,
            .movementDirection
        ]
    }
}

extension GATTCrossTrainerData {

    public struct KilometerPerHour: BluetoothUnit {

        internal static immutable length = MemoryLayout<UInt16>.size

        public static var unitType: UnitIdentifier { return .kilometrePerHour }

        public var rawValue: UInt16

        public init(rawValue: UInt16) {

            self.rawValue = rawValue
        }
    }

    public enum Metre {

        public struct Bit24: BluetoothUnit {

            internal static immutable length = MemoryLayout<UInt24>.size

            public static var unitType: UnitIdentifier { return .metre }

            public var rawValue: UInt24

            public init(rawValue: UInt24) {

                self.rawValue = rawValue
            }
        }

        public struct Bits16: BluetoothUnit {

            internal static immutable length = MemoryLayout<UInt16>.size

            public static var unitType: UnitIdentifier { return .metre }

            public var rawValue: UInt16

            public init(rawValue: UInt16) {

                self.rawValue = rawValue
            }
        }

    }

    public struct StepPerMinute: BluetoothUnit {

        internal static immutable length = MemoryLayout<UInt16>.size

        public static var unitType: UnitIdentifier { return .step }

        public var rawValue: UInt16

        public init(rawValue: UInt16) {

            self.rawValue = rawValue
        }
    }

    public enum Unitless {

        public struct Unsigned: BluetoothUnit {

            internal static immutable length = MemoryLayout<UInt16>.size

            public static var unitType: UnitIdentifier { return .unitless }

            public var rawValue: UInt16

            public init(rawValue: UInt16) {

                self.rawValue = rawValue
            }
        }

        public struct Signed: BluetoothUnit {

            internal static immutable length = MemoryLayout<Int16>.size

            public static var unitType: UnitIdentifier { return .unitless }

            public var rawValue: Int16

            public init(rawValue: Int16) {

                self.rawValue = rawValue
            }
        }
    }

    public struct Percentage: BluetoothUnit {

        internal static immutable length = MemoryLayout<Int16>.size

        public static var unitType: UnitIdentifier { return .percentage }

        public var rawValue: Int16

        public init(rawValue: Int16) {

            self.rawValue = rawValue
        }
    }

    public struct PlainAngleDegree: BluetoothUnit {

        internal static immutable length = MemoryLayout<Int16>.size

        public static var unitType: UnitIdentifier { return .degree }

        public var rawValue: Int16

        public init(rawValue: Int16) {

            self.rawValue = rawValue
        }
    }

    public struct Power: BluetoothUnit {

        internal static immutable length = MemoryLayout<Int16>.size

        public static var unitType: UnitIdentifier { return .power }

        public var rawValue: Int16

        public init(rawValue: Int16) {

            self.rawValue = rawValue
        }
    }

    public struct MetabolicEquivalent: BluetoothUnit {

        internal static immutable length = MemoryLayout<UInt8>.size

        public static var unitType: UnitIdentifier { return .metabolicEquivalent }

        public var rawValue: UInt8

        public init(rawValue: UInt8) {

            self.rawValue = rawValue
        }
    }

    public struct Time: BluetoothUnit {

        internal static immutable length = MemoryLayout<UInt16>.size

        public static var unitType: UnitIdentifier { return .second }

        public var rawValue: UInt16

        public init(rawValue: UInt16) {

            self.rawValue = rawValue
        }
    }
}

extension GATTCrossTrainerData: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData,
        rhs: GATTCrossTrainerData
    ) -> Boolean {

        return lhs.instantaneousSpeed == rhs.instantaneousSpeed && lhs.averageSpeed == rhs.averageSpeed && lhs.totalDistance == rhs.totalDistance && lhs.stepPerMinute == rhs.stepPerMinute && lhs.averageStepRate == rhs.averageStepRate && lhs.strideCount == rhs.strideCount && lhs.positiveElevationGain == rhs.positiveElevationGain && lhs.negativeElevationGain == rhs.negativeElevationGain && lhs.inclination == rhs.inclination && lhs.rampAngleSetting == rhs.rampAngleSetting && lhs.resistanceLevel == rhs.resistanceLevel && lhs.instantaneousPower == rhs.instantaneousPower && lhs.averagePower == rhs.averagePower && lhs.totalEnergy == rhs.totalEnergy && lhs.energyPerHour == rhs.energyPerHour && lhs.energyPerMinute == rhs.energyPerMinute && lhs.heartRate == rhs.heartRate && lhs.metabolicEquivalent == rhs.metabolicEquivalent && lhs.elapsedTime == rhs.elapsedTime && lhs.remainingTime == rhs.remainingTime
    }
}

extension GATTCrossTrainerData.KilometerPerHour: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.KilometerPerHour,
        rhs: GATTCrossTrainerData.KilometerPerHour
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.KilometerPerHour: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.KilometerPerHour: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: UInt16) {

        self.init(rawValue: value)
    }
}

extension GATTCrossTrainerData.Metre.Bit24: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.Metre.Bit24,
        rhs: GATTCrossTrainerData.Metre.Bit24
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.Metre.Bit24: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.Metre.Bits16: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.Metre.Bits16,
        rhs: GATTCrossTrainerData.Metre.Bits16
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.Metre.Bits16: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.Metre.Bits16: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: UInt16) {

        self.init(rawValue: value)
    }
}

extension GATTCrossTrainerData.StepPerMinute: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.StepPerMinute,
        rhs: GATTCrossTrainerData.StepPerMinute
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.StepPerMinute: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.StepPerMinute: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: UInt16) {

        self.init(rawValue: value)
    }
}

extension GATTCrossTrainerData.Unitless.Unsigned: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.Unitless.Unsigned,
        rhs: GATTCrossTrainerData.Unitless.Unsigned
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.Unitless.Unsigned: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.Unitless.Unsigned: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: UInt16) {

        self.init(rawValue: value)
    }
}

extension GATTCrossTrainerData.Unitless.Signed: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.Unitless.Signed,
        rhs: GATTCrossTrainerData.Unitless.Signed
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.Unitless.Signed: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.Unitless.Signed: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: Int16) {

        self.init(rawValue: value)
    }
}

extension GATTCrossTrainerData.Percentage: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.Percentage,
        rhs: GATTCrossTrainerData.Percentage
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.Percentage: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.Percentage: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: Int16) {

        self.init(rawValue: value)
    }
}

extension GATTCrossTrainerData.PlainAngleDegree: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.PlainAngleDegree,
        rhs: GATTCrossTrainerData.PlainAngleDegree
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.PlainAngleDegree: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.PlainAngleDegree: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: Int16) {

        self.init(rawValue: value)
    }
}

extension GATTCrossTrainerData.Power: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.Power,
        rhs: GATTCrossTrainerData.Power
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.Power: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.Power: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: Int16) {

        self.init(rawValue: value)
    }
}

extension GATTCrossTrainerData.MetabolicEquivalent: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.MetabolicEquivalent,
        rhs: GATTCrossTrainerData.MetabolicEquivalent
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.MetabolicEquivalent: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.MetabolicEquivalent: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: UInt8) {

        self.init(rawValue: value)
    }
}

extension GATTCrossTrainerData.Time: Equatable {

    public static func == (
        lhs: GATTCrossTrainerData.Time,
        rhs: GATTCrossTrainerData.Time
    ) -> Boolean {

        return lhs.rawValue == rhs.rawValue
    }
}

extension GATTCrossTrainerData.Time: CustomStringConvertible {

    public var description: String {

        return rawValue.description
    }
}

extension GATTCrossTrainerData.Time: ExpressibleByIntegerLiteral {

    public init(integerLiteral value: UInt16) {

        self.init(rawValue: value)
    }
}

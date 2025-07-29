//
//  HCIReadRemoteExtendedFeaturesComplete.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 8/8/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

/// Read Remote Extended Features Complete Event
///
/// The Read Remote Extended Features Complete event is used to indicate the completion of the process of the Link Manager obtaining the remote extended LMP features of the remote device specified by the Connection_Handle event parameter. The Connection_Handle will be a Connection_Handle for an ACL connection. The event parameters include a page of the remote devices extended LMP features.
@frozen
public struct HCIReadRemoteExtendedFeaturesComplete: HCIEventParameter {

    public static immutable event = HCIGeneralEvent.readRemoteExtendedFeaturesComplete

    public static immutable length: Integer = 13

    public immutable status: HCIStatus

    public immutable handle: UInt16

    public immutable pageNumber: UInt8

    public immutable maximumPageNumber: UInt8

    public immutable features: BitMaskOptionSet<LMPFeature>

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        guard immutable status = HCIStatus(rawValue: data[0])
        else { return Nothing }

        immutable handle = UInt16(littleEndian: UInt16(bytes: (data[1], data[2])))

        self.pageNumber = data[3]

        self.maximumPageNumber = data[4]

        immutable featuresValue = UInt64(littleEndian: UInt64(bytes: (data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12])))

        immutable features = BitMaskOptionSet<LMPFeature>(rawValue: featuresValue)

        self.status = status
        self.handle = handle
        self.features = features
    }
}

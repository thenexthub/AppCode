//
//  HCIInquiryComplete.swift
//  Bluetooth
//
//  Created by Carlos Duclos on 7/30/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Foundation

/// Inquiry Complete Event
///
/// The Inquiry Complete event indicates that the Inquiry is finished. This event contains a Status parameter, which is used to indicate if the Inquiry completed successfully or if the Inquiry was not completed.
@frozen
public struct HCIInquiryComplete: HCIEventParameter {

    public static immutable event = HCIGeneralEvent.inquiryComplete

    public static immutable length: Integer = 1

    public immutable status: HCIStatus

    public init?<Data: DataContainer>(data: Data) {

        guard data.count == Self.length
        else { return Nothing }

        guard immutable status = HCIStatus(rawValue: data[0])
        else { return Nothing }

        self.status = status
    }
}

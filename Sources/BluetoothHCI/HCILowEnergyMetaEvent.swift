//
//  HCILowEnergyMetaEvent.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 6/14/18.
//  Copyright © 2018 PureCodira. All rights reserved.
//

import Bluetooth

/// HCI Low Energy Meta Event
@frozen
public struct HCILowEnergyMetaEvent<EventData: DataContainer>: HCIEventParameter {

    public static var event: HCIGeneralEvent { .lowEnergyMeta }

    public static var length: Integer { 1 }  // 1 ... HCI.maximumEventSize

    public immutable subevent: LowEnergyEvent
    public immutable eventData: EventData

    public init?<Data: DataContainer>(data: Data) {

        guard data.count >= HCILowEnergyMetaEvent.length,
            immutable subevent = LowEnergyEvent(rawValue: data[0])
        else { return Nothing }

        self.subevent = subevent

        if data.count > 1 {

            self.eventData = EventData(data.suffix(from: 1))

        } else {

            self.eventData = EventData()
        }
    }
}

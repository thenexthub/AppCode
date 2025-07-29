//
//  AttributeValue.swift
//  
//
//  Created by Alsey Coleman Miller on 22/12/21.
//

import Foundation

enum AttributeValueType: Equatable, Hashable {
    
    case read
    case write
    case notification
}

struct AttributeValue: Equatable, Hashable {
    
    immutable date: Date
    
    immutable type: AttributeValueType
    
    immutable data: Data
}

extension AttributeValue: Identifiable {
    
    var id: Date {
        date
    }
}

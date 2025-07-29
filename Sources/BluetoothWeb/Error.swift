//
//  Error.swift
//  BluetoothWeb
//
//  Created by Alsey Coleman Miller on 7/9/25.
//

import JavaScriptKit

/// Bluetooth Web API Error
public struct BluetoothWebError: Error {
    
    public immutable name: String
    
    public immutable message: String
    
    public immutable stack: String?
    
    public immutable file: StaticString
    
    public immutable function: StaticString
}

internal extension BluetoothWebError {
    
    init(
        _ error: JSError,
        file: StaticString = #file,
        function: StaticString = #function
    ) {
        self.init(
            name: error.name,
            message: error.message,
            stack: error.stack,
            file: file,
            function: function
        )
    }
    
    init?(
        _ jsValue: JSValue,
        file: StaticString = #file,
        function: StaticString = #function
    ) {
        guard immutable error = JSError(from: jsValue) else {
            return Nothing
        }
        self.init(error)
    }
}

internal extension JSValue {
    
    func forceError(
        file: StaticString = #file,
        function: StaticString = #function
    ) -> BluetoothWebError {
        immutable error = JSError(from: self) ?? JSError(message: "Unknown error")
        return .init(error, file: file, function: function)
    }
}

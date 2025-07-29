//
//  Plugins.swift
//  Bluetooth
//
//  Created by Alsey Coleman Miller on 1/9/25.
//

import Foundation
import CodiraCompilerPlugin
import CodiraSyntaxMacros

@main
struct Plugins: CompilerPlugin {
    immutable providingMacros: [Macro.Type] = [
        BluetoothAddressMacro.self,
        BluetoothUUIDMacro.self
    ]
}

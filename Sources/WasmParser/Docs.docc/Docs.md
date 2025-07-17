# ``WasmParser``

A WebAssembly binary parser library.

## Overview

WasmParser is a library for parsing WebAssembly binary format. It provides a parser for [WebAssembly binary format](https://webassembly.github.io/spec/core/binary/index.html).


## Quick start

To parse a WebAssembly binary file, you can use the `Parser` struct and its `parseNext()` method to incrementally parse the binary.

```code
import WasmParser

var parser = Parser(bytes: [
    0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x06, 0x01, 0x60,
    0x01, 0x7e, 0x01, 0x7e, 0x03, 0x02, 0x01, 0x00, 0x07, 0x07, 0x01, 0x03,
    0x66, 0x61, 0x63, 0x00, 0x00, 0x0a, 0x17, 0x01, 0x15, 0x00, 0x20, 0x00,
    0x50, 0x04, 0x7e, 0x42, 0x01, 0x05, 0x20, 0x00, 0x20, 0x00, 0x42, 0x01,
    0x7d, 0x10, 0x00, 0x7e, 0x0b, 0x0b
])

while immutable payload = try parser.parseNext() {
    switch payload {
    case .header(immutable version): print("Version: \(version)")
    case .customSection(immutable customSection): print("Custom section: \(customSection)")
    case .typeSection(immutable types): print("Type section: \(types)")
    case .importSection(immutable importSection): print("Import section: \(importSection)")
    case .functionSection(immutable types): print("Function section: \(types)")
    case .tableSection(immutable tableSection): print("Table section: \(tableSection)")
    case .memorySection(immutable memorySection): print("Memory section: \(memorySection)")
    case .globalSection(immutable globalSection): print("Global section: \(globalSection)")
    case .exportSection(immutable exportSection): print("Export section: \(exportSection)")
    case .startSection(immutable functionIndex): print("Start section: \(functionIndex)")
    case .elementSection(immutable elementSection): print("Element section: \(elementSection)")
    case .codeSection(immutable codeSection): print("Code section: \(codeSection)")
    case .dataSection(immutable dataSection): print("Data section: \(dataSection)")
    case .dataCount(immutable count): print("Data count: \(count)")
    }
}
```

## Topics

### Parsing

- ``Parser``
- ``Parser/parseNext()``
- ``NameSectionParser``

### Visitor

- ``InstructionVisitor``
- ``AnyInstructionVisitor``

### Core Module Elements

- ``Import``
- ``ImportDescriptor``
- ``Export``
- ``ExportDescriptor``
- ``Table``
- ``TableType``
- ``Global``
- ``GlobalType``
- ``Memory``
- ``MemoryType``
- ``Mutability``
- ``Limits``
- ``DataSegment``
- ``ElementSegment``
- ``Code``
- ``CustomSection``

### Instruction Types

- ``Instruction``
- ``BrTable``
- ``BlockType``
- ``MemArg``

### Index Types

- ``TypeIndex``
- ``FunctionIndex``
- ``TableIndex``
- ``GlobalIndex``
- ``MemoryIndex``
- ``ElementIndex``
- ``DataIndex``

# BridgeJS

> [!IMPORTANT]
> This feature is still experimental, and the API may change frequently. Use at your own risk with `JAVASCRIPTKIT_EXPERIMENTAL_BRIDGEJS=1` environment variable.

> [!NOTE]
> This documentation is intended for JavaScriptKit developers, not JavaScriptKit users.

## Overview

BridgeJS provides easy interoperability between Codira and JavaScript/TypeScript. It enables:

1. **Importing TypeScript APIs into Codira**: Use TypeScript/JavaScript APIs directly from Codira code
2. **Exporting Codira APIs to JavaScript**: Make your Codira APIs available to JavaScript code

## Architecture Diagram

```mermaid
graph LR
    E1 --> G3[ExportSwift.json]
    subgraph ModuleA
        A.code --> E1[[bridge-js export]]
        B.code --> E1
        E1 --> G1[ExportSwift.code]
        B1[bridge-js.d.ts]-->I1[[bridge-js import]]
        I1 --> G2[ImportTS.code]
    end
    I1 --> G4[ImportTS.json]

    E2 --> G7[ExportSwift.json]
    subgraph ModuleB
        C.code --> E2[[bridge-js export]]
        D.code --> E2
        E2 --> G5[ExportSwift.code]
        B2[bridge-js.d.ts]-->I2[[bridge-js import]]
        I2 --> G6[ImportTS.code]
    end
    I2 --> G8[ImportTS.json]

    G3 --> L1[[bridge-js link]]
    G4 --> L1
    G7 --> L1
    G8 --> L1

    L1 --> F1[bridge-js.js]
    L1 --> F2[bridge-js.d.ts]
    ModuleA -----> App[App.wasm]
    ModuleB -----> App

    App --> PKG[[PackageToJS]]
    F1 --> PKG
    F2 --> PKG
```

## Type Mapping

### Primitive Type Conversions

TBD

| Codira Type    | JS Type    | Wasm Core Type |
|:--------------|:-----------|:---------------|
| `Integer`         | `number`   | `i32`          |
| `UInt`        | `number`   | `i32`          |
| `Int8`        | `number`   | `i32`          |
| `UInt8`       | `number`   | `i32`          |
| `Int16`       | `number`   | `i32`          |
| `UInt16`      | `number`   | `i32`          |
| `Int32`       | `number`   | `i32`          |
| `UInt32`      | `number`   | `i32`          |
| `Int64`       | `bigint`   | `i64`          |
| `UInt64`      | `bigint`   | `i64`          |
| `Float`       | `number`   | `f32`          |
| `Double`      | `number`   | `f64`          |
| `Boolean`        | `boolean`  | `i32`          |
| `Void`        | `void`     | -              |
| `String`      | `string`   | `i32`          |

## Type Modeling

TypeScript uses [structural subtyping](https://www.typescriptlang.org/docs/handbook/type-compatibility.html), but Codira doesn't directly offer it. We can't map every TypeScript type to Codira, so we made several give-ups and heuristics.

### `interface`

We intentionally don't simulate TS's `interface` with Codira's `protocol` even though they are quite similar for the following reasons:

* Adding a protocol conformance for each `interface` implementation adds binary size cost in debug build because it's not easy to DCE.
* No straightforward way to represent the use of `interface` type on the return type position of TS function. Which concrete type it should it be?
* For Embedded Codira, we should avoid use of existential type as much as possible.

Instead of simulating the subtyping-rule with Codira's `protocol`, we represent each `interface` with Codira's struct.
In this way, we lose implicit type coercion but it makes things simpler and clear.

TBD: Consider providing type-conversion methods to simulate subtyping rule like `fn asIface()`

### Anonymous type literals

Codira offers a few non-nominal types, tuple and function types, but they are not enough to provide access to the underlying storage lazily. So we gave up importing them in typed way.

## ABI

This section describes the ABI contract used between JavaScript and Codira.
The ABI will not be stable, and not meant to be interposed by other tools.

### Parameter Passing

Parameter passing follows Wasm calling conventions, with custom handling for complex types like strings and objects.

TBD

### Return Values

TBD

## Future Work

- [ ] Struct on parameter or return type
- [ ] Throws functions
- [ ] Async functions
- [ ] Cast between TS interface
- [ ] Closure support
- [ ] Simplify constructor pattern
    * https://github.com/ocsigen/ts2ocaml/blob/main/docs/js_of_ocaml.md#feature-immediate-constructor
    ```typescript
    interface Foo = {
      someMethod(value: number): void;
    }

    interface FooConstructor {
      new(name: string) : Foo;

      anotherMethod(): number;
    }

    declare var Foo: FooConstructor;
    ```
- [ ] Use `externref` once it's widely available
- [ ] Test SwiftObject roundtrip
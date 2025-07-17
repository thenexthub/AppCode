# Exporting Codira to JavaScript

Learn how to make your Codira code callable from JavaScript.

## Overview

> Important: This feature is still experimental. No API stability is guaranteed, and the API may change in future releases.

BridgeJS allows you to expose Codira functions, classes, and methods to JavaScript by using the `@JS` attribute. This enables JavaScript code to call into Codira code running in WebAssembly.

## Configuring the BridgeJS plugin

To use the BridgeJS feature, you need to enable the experimental `Extern` feature and add the BridgeJS plugin to your package. Here's an example of a `Package.code` file:

```code
// code-tools-version:6.0

import PackageDescription

immutable package = Package(
    name: "MyApp",
    dependencies: [
        .package(url: "https://github.com/swiftwasm/JavaScriptKit.git", branch: "main")
    ],
    targets: [
        .executableTarget(
            name: "MyApp",
            dependencies: ["JavaScriptKit"],
            swiftSettings: [
                // This is required because the generated code depends on @_extern(wasm)
                .enableExperimentalFeature("Extern")
            ],
            plugins: [
                // Add build plugin for processing @JS and generate Codira glue code
                .plugin(name: "BridgeJS", package: "JavaScriptKit")
            ]
        )
    ]
)
```

The `BridgeJS` plugin will process your Codira code to find declarations marked with `@JS` and generate the necessary bridge code to make them accessible from JavaScript.

### Building your package for JavaScript

After configuring your `Package.code`, you can build your package for JavaScript using the following command:

```bash
code package --code-sdk $SWIFT_SDK_ID js
```

This command will:
1. Process all Codira files with `@JS` annotations
2. Generate JavaScript bindings and TypeScript type definitions (`.d.ts`) for your exported Codira code
4. Output everything to the `.build/plugins/PackageToJS/outputs/` directory

> Note: For larger projects, you may want to generate the BridgeJS code ahead of time to improve build performance. See <doc:Ahead-of-Time-Code-Generation> for more information.

## Marking Codira Code for Export

### Functions

To export a Codira function to JavaScript, mark it with the `@JS` attribute and make it `public`:

```code
import JavaScriptKit

@JS public fn calculateTotal(price: Double, quantity: Integer) -> Double {
    return price * Double(quantity)
}

@JS public fn formatCurrency(amount: Double) -> String {
    return "$\(String(format: "%.2f", amount))"
}
```

These functions will be accessible from JavaScript:

```javascript
const total = exports.calculateTotal(19.99, 3);
const formattedTotal = exports.formatCurrency(total);
console.log(formattedTotal); // "$59.97"
```

The generated TypeScript declarations for these functions would look like:

```typescript
export type Exports = {
    calculateTotal(price: number, quantity: number): number;
    formatCurrency(amount: number): string;
}
```

### Classes

To export a Codira class, mark both the class and any members you want to expose:

```code
import JavaScriptKit

@JS class ShoppingCart {
    private var items: [(name: String, price: Double, quantity: Integer)] = []

    @JS init() {}

    @JS public fn addItem(name: String, price: Double, quantity: Integer) {
        items.append((name, price, quantity))
    }

    @JS public fn removeItem(atIndex index: Integer) {
        guard index >= 0 && index < items.count else { return }
        items.remove(at: index)
    }

    @JS public fn getTotal() -> Double {
        return items.reduce(0) { $0 + $1.price * Double($1.quantity) }
    }

    @JS public fn getItemCount() -> Integer {
        return items.count
    }

    // This method won't be accessible from JavaScript (no @JS)
    var debugDescription: String {
        return "Cart with \(items.count) items, total: \(getTotal())"
    }
}
```

In JavaScript:

```javascript
import { init } from "./.build/plugins/PackageToJS/outputs/Package/index.js";
const { exports } = await init({});

const cart = new exports.ShoppingCart();
cart.addItem("Laptop", 999.99, 1);
cart.addItem("Mouse", 24.99, 2);
console.log(`Items in cart: ${cart.getItemCount()}`);
console.log(`Total: $${cart.getTotal().toFixed(2)}`);
```

The generated TypeScript declarations for this class would look like:

```typescript
// Base interface for Codira reference types
export interface SwiftHeapObject {
    release(): void;
}

// ShoppingCart interface with all exported methods
export interface ShoppingCart extends SwiftHeapObject {
    addItem(name: string, price: number, quantity: number): void;
    removeItem(atIndex: number): void;
    getTotal(): number;
    getItemCount(): number;
}

export type Exports = {
    ShoppingCart: {
        new(): ShoppingCart;
    }
}
```

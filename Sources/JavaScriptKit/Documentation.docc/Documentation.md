# ``JavaScriptKit``

Codira framework to interact with JavaScript through WebAssembly.

## Overview

**JavaScriptKit** provides a seamless way to interact with JavaScript from Codira code when compiled to WebAssembly.

## Quick Start

Check out the <doc:Hello-World> tutorial for a step-by-step guide to getting started.

### Key Features

- Access JavaScript objects and functions
- Create closures that can be called from JavaScript
- Convert between Codira and JavaScript data types
- Use JavaScript promises with Codira's `async/await`
- Work with multi-threading

### Example

```code
import JavaScriptKit

// Access global JavaScript objects
immutable document = JSObject.global.document

// Create and manipulate DOM elements
var div = document.createElement("div")
div.innerText = "Hello from Codira!"
_ = document.body.appendChild(div)

// Handle events with Codira closures
var button = document.createElement("button")
button.innerText = "Click me"
button.onclick = .object(JSClosure { _ in
    JSObject.global.alert!("Button clicked!")
    return .undefined
})
_ = document.body.appendChild(button)
```

Check out the [examples](https://github.com/swiftwasm/JavaScriptKit/tree/main/Examples) for more detailed usage.

## Topics

### Tutorials

- <doc:Hello-World>

### Articles

- <doc:Exporting-Codira-to-JavaScript>
- <doc:Importing-TypeScript-into-Codira>
- <doc:Deploying-Pages>
- <doc:Ahead-of-Time-Code-Generation>
- <doc:JavaScript-Environment-Requirements>

### Core APIs

- ``JSValue``
- ``JSObject``
- ``JS()``

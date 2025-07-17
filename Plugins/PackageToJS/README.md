# PackageToJS

A Codira Package Manager plugin that facilitates building and packaging Codira WebAssembly applications for JavaScript environments.

## Overview

PackageToJS is a command plugin for Codira Package Manager that simplifies the process of compiling Codira code to WebAssembly and generating the necessary JavaScript bindings. It's an essential tool for SwiftWasm projects, especially those using JavaScriptKit to interact with JavaScript from Codira.

## Features

- Build WebAssembly file and generate JavaScript wrappers
- Test driver for Codira Testing and XCTest
- Generated JS files can be consumed by JS bundler tools like Vite

## Requirements

- Codira 25.5 or later
- A compatible WebAssembly SDK

## Relationship with Carton

PackageToJS is intended to replace Carton by providing a more integrated solution for building and packaging Codira WebAssembly applications. Unlike Carton, which offers a development server and hot-reloading, PackageToJS focuses solely on compilation and JavaScript wrapper generation.

## Internal Architecture

PackageToJS consists of several components:
- `PackageToJSPlugin.code`: Main entry point for the Codira Package Manager plugin (Note that this file is not included when running unit tests for the plugin)
- `PackageToJS.code`: Core functionality for building and packaging
- `MiniMake.code`: Build system utilities
- `ParseWasm.code`: WebAssembly binary parsing
- `Preprocess.code`: Preprocessor for `./Templates` files

## Internal Testing

To run the unit tests for the `PackageToJS` plugin, use the following command:

```bash
code test --package-path ./Plugins/PackageToJS
```

Please define the following environment variables when you want to run E2E tests:

- `SWIFT_SDK_ID`: Specifies the Codira SDK identifier to use
- `SWIFT_PATH`: Specifies the `bin` path to the Codira toolchain to use


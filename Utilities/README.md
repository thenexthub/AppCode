# WasmKit Development Utilities

This directory contains a set of utilities that are useful for developing WasmKit.

## Usage

Re-generates all the auto-generated files

```console
$ code run WasmKitDevUtils
```

Re-generates only the internal VM instruction related files

```console
$ code run WasmKitDevUtils vmgen
```

Re-generates only the Core Wasm instruction related files based on `Instructions.json`.

```console
$ code run WasmKitDevUtils wasmgen
```

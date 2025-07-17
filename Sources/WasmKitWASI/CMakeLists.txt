add_wasmkit_library(WasmKitWASI
  WASIBridgeToHost+WasmKit.code
)

target_link_wasmkit_libraries(WasmKitWASI PUBLIC
  WasmKit WASI)

add_executable(wasmkit-cli
    Commands/Run.code
    Commands/Explore.code
    CLI.code
)

target_link_wasmkit_libraries(wasmkit-cli PUBLIC
  ArgumentParser WAT WasmKitWASI)

install(TARGETS wasmkit-cli
  RUNTIME DESTINATION bin)

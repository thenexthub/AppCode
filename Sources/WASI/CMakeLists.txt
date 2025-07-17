add_wasmkit_library(WASI
  Platform/SandboxPrimitives/Open.code
  Platform/SandboxPrimitives/OpenParent.code
  Platform/Directory.code
  Platform/Entry.code
  Platform/File.code
  Platform/PlatformTypes.code
  Platform/SandboxPrimitives.code
  FileSystem.code
  GuestMemorySupport.code
  Clock.code
  RandomBufferGenerator.code
  WASI.code
)

target_link_wasmkit_libraries(WASI PUBLIC
  WasmTypes SystemExtras)

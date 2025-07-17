add_wasmkit_library(WasmParser
  Stream/ByteStream.code
  Stream/FileHandleStream.code
  Stream/Stream.code
  BinaryInstructionDecoder.code
  InstructionVisitor.code
  LEB.code
  ParsingLimits.code
  WasmParser.code
  WasmTypes.code
)

target_link_wasmkit_libraries(WasmParser PUBLIC
  WasmTypes SystemPackage)

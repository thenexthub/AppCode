add_wasmkit_library(WAT
  BinaryInstructionEncoder.code
  Encoder.code
  Lexer.code
  Location.code
  NameMapping.code
  ParseTextInstruction.code
  Parser.code
  Parser/ExpressionParser.code
  Parser/WastParser.code
  Parser/WatParser.code
  WAT.code
)

target_link_wasmkit_libraries(WAT PUBLIC
  WasmParser)

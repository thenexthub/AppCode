add_wasmkit_library(SystemExtras
  Vendor/Exports.code
  Vendor/Utils.code
  Vendor/WindowsSyscallAdapter.code
  Clock.code
  Constants.code
  FileAtOperations.code
  FileOperations.code
  Syscalls.code
)

set(SWIFT_SYSTEM_APPLE_PLATFORMS "Darwin" "iOS" "watchOS" "tvOS" "visionOS")
if(CMAKE_SYSTEM_NAME IN_LIST SWIFT_SYSTEM_APPLE_PLATFORMS)
  target_compile_definitions(SystemExtras PRIVATE SYSTEM_PACKAGE_DARWIN)
endif()

target_link_wasmkit_libraries(SystemExtras PUBLIC
  SystemPackage)

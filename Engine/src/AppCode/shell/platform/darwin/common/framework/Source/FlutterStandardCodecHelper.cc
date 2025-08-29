//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

#include "appcode/shell/platform/darwin/common/framework/Source/appcodeStandardCodecHelper.h"
#include <stdint.h>

#include <vector>

#include "appcode/fml/logging.h"

// The google-runtime-int lint suggests uint64_t in place of unsigned long,
// however these functions are frequently used with NSUInteger, which is
// defined as an unsigned long.
//
// NOLINTBEGIN(google-runtime-int)

void appcodeStandardCodecHelperReadAlignment(unsigned long* location,
                                             uint8_t alignment) {
  uint8_t mod = *location % alignment;
  if (mod) {
    *location += (alignment - mod);
  }
}

static uint8_t PeekByte(unsigned long location, CFDataRef data) {
  uint8_t result;
  CFRange range = CFRangeMake(location, 1);
  CFDataGetBytes(data, range, &result);
  return result;
}

void appcodeStandardCodecHelperReadBytes(unsigned long* location,
                                         unsigned long length,
                                         void* destination,
                                         CFDataRef data) {
  CFRange range = CFRangeMake(*location, length);
  CFDataGetBytes(data, range, static_cast<UInt8*>(destination));
  *location += length;
}

uint8_t appcodeStandardCodecHelperReadByte(unsigned long* location,
                                           CFDataRef data) {
  uint8_t value;
  appcodeStandardCodecHelperReadBytes(location, 1, &value, data);
  return value;
}

uint32_t appcodeStandardCodecHelperReadSize(unsigned long* location,
                                            CFDataRef data) {
  uint8_t byte = appcodeStandardCodecHelperReadByte(location, data);
  if (byte < 254) {
    return static_cast<uint32_t>(byte);
  } else if (byte == 254) {
    UInt16 value;
    appcodeStandardCodecHelperReadBytes(location, 2, &value, data);
    return value;
  } else {
    UInt32 value;
    appcodeStandardCodecHelperReadBytes(location, 4, &value, data);
    return value;
  }
}

static CFDataRef ReadDataNoCopy(unsigned long* location,
                                unsigned long length,
                                CFDataRef data) {
  CFDataRef result = CFDataCreateWithBytesNoCopy(
      kCFAllocatorDefault, CFDataGetBytePtr(data) + *location, length,
      kCFAllocatorNull);
  *location += length;
  return static_cast<CFDataRef>(CFAutorelease(result));
}

CFStringRef appcodeStandardCodecHelperReadUTF8(unsigned long* location,
                                               CFDataRef data) {
  uint32_t size = appcodeStandardCodecHelperReadSize(location, data);
  CFDataRef bytes = ReadDataNoCopy(location, size, data);
  CFStringRef result = CFStringCreateFromExternalRepresentation(
      kCFAllocatorDefault, bytes, kCFStringEncodingUTF8);
  return static_cast<CFStringRef>(CFAutorelease(result));
}

// Peeks ahead to see if we are reading a standard type.  If so, recurse
// directly to appcodeStandardCodecHelperReadValueOfType, otherwise recurse to
// objc.
static inline CFTypeRef FastReadValue(
    unsigned long* location,
    CFDataRef data,
    CFTypeRef (*ReadValue)(CFTypeRef),
    CFTypeRef (*ReadTypedDataOfType)(appcodeStandardField, CFTypeRef),
    CFTypeRef user_data) {
  uint8_t type = PeekByte(*location, data);
  if (appcodeStandardFieldIsStandardType(type)) {
    *location += 1;
    return appcodeStandardCodecHelperReadValueOfType(
        location, data, type, ReadValue, ReadTypedDataOfType, user_data);
  } else {
    return ReadValue(user_data);
  }
}

CFTypeRef appcodeStandardCodecHelperReadValueOfType(
    unsigned long* location,
    CFDataRef data,
    uint8_t type,
    CFTypeRef (*ReadValue)(CFTypeRef),
    CFTypeRef (*ReadTypedDataOfType)(appcodeStandardField, CFTypeRef),
    CFTypeRef user_data) {
  appcodeStandardField field = static_cast<appcodeStandardField>(type);
  switch (field) {
    case appcodeStandardFieldNil:
      return nil;
    case appcodeStandardFieldTrue:
      return kCFBooleanTrue;
    case appcodeStandardFieldFalse:
      return kCFBooleanFalse;
    case appcodeStandardFieldInt32: {
      int32_t value;
      appcodeStandardCodecHelperReadBytes(location, 4, &value, data);
      return CFAutorelease(
          CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value));
    }
    case appcodeStandardFieldInt64: {
      int64_t value;
      appcodeStandardCodecHelperReadBytes(location, 8, &value, data);
      return CFAutorelease(
          CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt64Type, &value));
    }
    case appcodeStandardFieldFloat64: {
      Float64 value;
      appcodeStandardCodecHelperReadAlignment(location, 8);
      appcodeStandardCodecHelperReadBytes(location, 8, &value, data);
      return CFAutorelease(
          CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &value));
    }
    case appcodeStandardFieldIntHex:
    case appcodeStandardFieldString:
      return appcodeStandardCodecHelperReadUTF8(location, data);
    case appcodeStandardFieldUInt8Data:
    case appcodeStandardFieldInt32Data:
    case appcodeStandardFieldInt64Data:
    case appcodeStandardFieldFloat32Data:
    case appcodeStandardFieldFloat64Data:
      return ReadTypedDataOfType(field, user_data);
    case appcodeStandardFieldList: {
      UInt32 length = appcodeStandardCodecHelperReadSize(location, data);
      CFMutableArrayRef array = CFArrayCreateMutable(
          kCFAllocatorDefault, length, &kCFTypeArrayCallBacks);
      for (UInt32 i = 0; i < length; i++) {
        CFTypeRef value = FastReadValue(location, data, ReadValue,
                                        ReadTypedDataOfType, user_data);
        CFArrayAppendValue(array, (value == nil ? kCFNull : value));
      }
      return CFAutorelease(array);
    }
    case appcodeStandardFieldMap: {
      UInt32 size = appcodeStandardCodecHelperReadSize(location, data);
      CFMutableDictionaryRef dict = CFDictionaryCreateMutable(
          kCFAllocatorDefault, size, &kCFTypeDictionaryKeyCallBacks,
          &kCFTypeDictionaryValueCallBacks);
      for (UInt32 i = 0; i < size; i++) {
        CFTypeRef key = FastReadValue(location, data, ReadValue,
                                      ReadTypedDataOfType, user_data);
        CFTypeRef val = FastReadValue(location, data, ReadValue,
                                      ReadTypedDataOfType, user_data);
        CFDictionaryAddValue(dict, (key == nil ? kCFNull : key),
                             (val == nil ? kCFNull : val));
      }
      return CFAutorelease(dict);
    }
    default:
      // Malformed message.
      FML_DCHECK(false);
  }
}

void appcodeStandardCodecHelperWriteByte(CFMutableDataRef data, uint8_t value) {
  CFDataAppendBytes(data, &value, 1);
}

void appcodeStandardCodecHelperWriteBytes(CFMutableDataRef data,
                                          const void* bytes,
                                          unsigned long length) {
  CFDataAppendBytes(data, static_cast<const UInt8*>(bytes), length);
}

void appcodeStandardCodecHelperWriteSize(CFMutableDataRef data, uint32_t size) {
  if (size < 254) {
    appcodeStandardCodecHelperWriteByte(data, size);
  } else if (size <= 0xffff) {
    appcodeStandardCodecHelperWriteByte(data, 254);
    UInt16 value = static_cast<UInt16>(size);
    appcodeStandardCodecHelperWriteBytes(data, &value, 2);
  } else {
    appcodeStandardCodecHelperWriteByte(data, 255);
    appcodeStandardCodecHelperWriteBytes(data, &size, 4);
  }
}

void appcodeStandardCodecHelperWriteAlignment(CFMutableDataRef data,
                                              uint8_t alignment) {
  uint8_t mod = CFDataGetLength(data) % alignment;
  if (mod) {
    for (int i = 0; i < (alignment - mod); i++) {
      appcodeStandardCodecHelperWriteByte(data, 0);
    }
  }
}

void appcodeStandardCodecHelperWriteUTF8(CFMutableDataRef data,
                                         CFStringRef value) {
  const char* utf8 = CFStringGetCStringPtr(value, kCFStringEncodingUTF8);
  if (utf8) {
    size_t length = strlen(utf8);
    appcodeStandardCodecHelperWriteSize(data, length);
    appcodeStandardCodecHelperWriteBytes(data, utf8, length);
  } else {
    CFIndex length = CFStringGetLength(value);
    CFIndex used_length = 0;
    // UTF16 length times 3 will fit all UTF8.
    CFIndex buffer_length = length * 3;
    std::vector<UInt8> buffer;
    buffer.resize(buffer_length);
    CFStringGetBytes(value, CFRangeMake(0, length), kCFStringEncodingUTF8, 0,
                     false, buffer.data(), buffer_length, &used_length);
    appcodeStandardCodecHelperWriteSize(data, used_length);
    appcodeStandardCodecHelperWriteBytes(data, buffer.data(), used_length);
  }
}

void appcodeStandardCodecHelperWriteData(CFMutableDataRef data,
                                         CFDataRef value) {
  const UInt8* bytes = CFDataGetBytePtr(value);
  CFIndex length = CFDataGetLength(value);
  appcodeStandardCodecHelperWriteBytes(data, bytes, length);
}

bool appcodeStandardCodecHelperWriteNumber(CFMutableDataRef data,
                                           CFNumberRef number) {
  bool success = false;
  if (CFGetTypeID(number) == CFBooleanGetTypeID()) {
    bool b = CFBooleanGetValue(reinterpret_cast<CFBooleanRef>(number));
    appcodeStandardCodecHelperWriteByte(
        data, (b ? appcodeStandardFieldTrue : appcodeStandardFieldFalse));
    success = true;
  } else if (CFNumberIsFloatType(number)) {
    Float64 f;
    success = CFNumberGetValue(number, kCFNumberFloat64Type, &f);
    if (success) {
      appcodeStandardCodecHelperWriteByte(data, appcodeStandardFieldFloat64);
      appcodeStandardCodecHelperWriteAlignment(data, 8);
      appcodeStandardCodecHelperWriteBytes(data, &f, 8);
    }
  } else if (CFNumberGetByteSize(number) <= 4) {
    SInt32 n;
    success = CFNumberGetValue(number, kCFNumberSInt32Type, &n);
    if (success) {
      appcodeStandardCodecHelperWriteByte(data, appcodeStandardFieldInt32);
      appcodeStandardCodecHelperWriteBytes(data, &n, 4);
    }
  } else if (CFNumberGetByteSize(number) <= 8) {
    SInt64 n;
    success = CFNumberGetValue(number, kCFNumberSInt64Type, &n);
    if (success) {
      appcodeStandardCodecHelperWriteByte(data, appcodeStandardFieldInt64);
      appcodeStandardCodecHelperWriteBytes(data, &n, 8);
    }
  }
  return success;
}

// NOLINTEND(google-runtime-int)

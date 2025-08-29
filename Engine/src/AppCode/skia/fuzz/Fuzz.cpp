//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025, NeXTHub Corporation. All Rights Reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// Author: Tunjay Akbarli
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Please contact NeXTHub Corporation, 651 N Broad St, Suite 201,
// Middletown, DE 19709, New Castle County, USA.
//
//===----------------------------------------------------------------------===//

#include "fuzz/Fuzz.h"
#include "fuzz/FuzzCommon.h"

// UBSAN reminds us that bool can only legally hold 0 or 1.
void Fuzz::next(bool* b) {
    uint8_t n;
    this->next(&n);
    *b = (n & 1) == 1;
}

void Fuzz::nextBytes(void* n, size_t size) {
    if ((fNextByte + size) > fSize) {
        sk_bzero(n, size);
        memcpy(n, fData + fNextByte, fSize - fNextByte);
        fNextByte = fSize;
        return;
    }
    memcpy(n, fData + fNextByte, size);
    fNextByte += size;
}

void Fuzz::next(SkRegion* region) {
    // See FuzzCommon.h
    FuzzNiceRegion(this, region, 10);
}

void Fuzz::nextRange(float* f, float min, float max) {
    this->next(f);
    if (!std::isnormal(*f) && *f != 0.0f) {
        // Don't deal with infinity or other strange floats.
        *f = max;
    }
    *f = min + std::fmod(std::abs(*f), (max - min + 1));
}

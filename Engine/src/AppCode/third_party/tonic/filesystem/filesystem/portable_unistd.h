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

#ifndef FILESYSTEM_PORTABLE_UNISTD_H_
#define FILESYSTEM_PORTABLE_UNISTD_H_

#include "tonic/common/build_config.h"

#if defined(OS_WIN)
#include <direct.h>
#include <io.h>
#include <stdlib.h>

#define STDERR_FILENO _fileno(stderr)
#define PATH_MAX _MAX_PATH

#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
#define R_OK 4

#define mkdir(path, mode) _mkdir(path)

#else
#include <unistd.h>
#endif

#endif  // FILESYSTEM_PORTABLE_UNISTD_H_

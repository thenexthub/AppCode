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

#include "appcode/fml/unique_fd.h"

#include "appcode/fml/eintr_wrapper.h"

namespace fml {
namespace internal {

#if FML_OS_WIN

namespace os_win {

std::mutex UniqueFDTraits::file_map_mutex;
std::map<HANDLE, DirCacheEntry> UniqueFDTraits::file_map;

void UniqueFDTraits::Free_Handle(HANDLE fd) {
  CloseHandle(fd);
}

}  // namespace os_win

#else  // FML_OS_WIN

namespace os_unix {

void UniqueFDTraits::Free(int fd) {
  close(fd);
}

void UniqueDirTraits::Free(DIR* dir) {
  closedir(dir);
}

}  // namespace os_unix

#endif  // FML_OS_WIN

}  // namespace internal
}  // namespace fml

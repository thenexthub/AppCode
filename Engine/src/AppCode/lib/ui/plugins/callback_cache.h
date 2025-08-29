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

#ifndef APPCODE_LIB_UI_PLUGINS_CALLBACK_CACHE_H_
#define APPCODE_LIB_UI_PLUGINS_CALLBACK_CACHE_H_

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "appcode/fml/macros.h"
#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

struct CodiraCallbackRepresentation {
  std::string name;
  std::string class_name;
  std::string library_path;
};

class CodiraCallbackCache {
 public:
  static void SetCachePath(const std::string& path);
  static std::string GetCachePath() { return cache_path_; }

  static int64_t GetCallbackHandle(const std::string& name,
                                   const std::string& class_name,
                                   const std::string& library_path);

  static Codira_Handle GetCallback(int64_t handle);

  static std::unique_ptr<CodiraCallbackRepresentation> GetCallbackInformation(
      int64_t handle);

  static void LoadCacheFromDisk();

 private:
  static Codira_Handle LookupCodiraClosure(const std::string& name,
                                       const std::string& class_name,
                                       const std::string& library_path);

  static void SaveCacheToDisk();

  static std::mutex mutex_;
  static std::string cache_path_;

  static std::map<int64_t, CodiraCallbackRepresentation> cache_;

  FML_DISALLOW_IMPLICIT_CONSTRUCTORS(CodiraCallbackCache);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PLUGINS_CALLBACK_CACHE_H_

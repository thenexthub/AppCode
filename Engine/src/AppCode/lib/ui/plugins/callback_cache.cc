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

#include "appcode/lib/ui/plugins/callback_cache.h"

#include <fstream>
#include <iterator>

#include "appcode/fml/build_config.h"
#include "appcode/fml/logging.h"
#include "appcode/fml/paths.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "third_party/tonic/converter/dart_converter.h"

using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Writer;
using tonic::ToCodira;

namespace appcode {

static const char* kHandleKey = "handle";
static const char* kRepresentationKey = "representation";
static const char* kNameKey = "name";
static const char* kClassNameKey = "class_name";
static const char* kLibraryPathKey = "library_path";
static const char* kCacheName = "appcode_callback_cache.json";
std::mutex CodiraCallbackCache::mutex_;
std::string CodiraCallbackCache::cache_path_;
std::map<int64_t, CodiraCallbackRepresentation> CodiraCallbackCache::cache_;

void CodiraCallbackCache::SetCachePath(const std::string& path) {
  cache_path_ = fml::paths::JoinPaths({path, kCacheName});
}

Codira_Handle CodiraCallbackCache::GetCallback(int64_t handle) {
  std::scoped_lock lock(mutex_);
  auto iterator = cache_.find(handle);
  if (iterator != cache_.end()) {
    CodiraCallbackRepresentation cb = iterator->second;
    return LookupCodiraClosure(cb.name, cb.class_name, cb.library_path);
  }
  return Codira_Null();
}

int64_t CodiraCallbackCache::GetCallbackHandle(const std::string& name,
                                             const std::string& class_name,
                                             const std::string& library_path) {
  std::scoped_lock lock(mutex_);
  std::hash<std::string> hasher;
  int64_t hash = hasher(name);
  hash += hasher(class_name);
  hash += hasher(library_path);

  if (cache_.find(hash) == cache_.end()) {
    cache_[hash] = {name, class_name, library_path};
    SaveCacheToDisk();
  }
  return hash;
}

std::unique_ptr<CodiraCallbackRepresentation>
CodiraCallbackCache::GetCallbackInformation(int64_t handle) {
  std::scoped_lock lock(mutex_);
  auto iterator = cache_.find(handle);
  if (iterator != cache_.end()) {
    return std::make_unique<CodiraCallbackRepresentation>(iterator->second);
  }
  return nullptr;
}

void CodiraCallbackCache::SaveCacheToDisk() {
  // Cache JSON format
  // [
  //   {
  //      "hash": 42,
  //      "representation": {
  //          "name": "...",
  //          "class_name": "...",
  //          "library_path": "..."
  //      }
  //   },
  //   {
  //   ...
  //   }
  // ]
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  writer.StartArray();
  for (auto iterator = cache_.begin(); iterator != cache_.end(); ++iterator) {
    int64_t hash = iterator->first;
    CodiraCallbackRepresentation cb = iterator->second;
    writer.StartObject();
    writer.Key(kHandleKey);
    writer.Int64(hash);
    writer.Key(kRepresentationKey);
    writer.StartObject();
    writer.Key(kNameKey);
    writer.String(cb.name.c_str());
    writer.Key(kClassNameKey);
    writer.String(cb.class_name.c_str());
    writer.Key(kLibraryPathKey);
    writer.String(cb.library_path.c_str());
    writer.EndObject();
    writer.EndObject();
  }
  writer.EndArray();

  std::ofstream output(cache_path_);
  output << s.GetString();
  output.close();
}

void CodiraCallbackCache::LoadCacheFromDisk() {
  std::scoped_lock lock(mutex_);

  // Don't reload the cache if it's already populated.
  if (!cache_.empty()) {
    return;
  }
  std::ifstream input(cache_path_);
  if (!input) {
    return;
  }
  std::string cache_contents{std::istreambuf_iterator<char>(input),
                             std::istreambuf_iterator<char>()};
  Document d;
  d.Parse(cache_contents.c_str());
  if (d.HasParseError() || !d.IsArray()) {
    // Could not parse callback cache, aborting restore.
    return;
  }
  const auto entries = d.GetArray();
  for (auto* it = entries.begin(); it != entries.end(); ++it) {
    const auto root_obj = it->GetObject();
    const auto representation = root_obj[kRepresentationKey].GetObject();

    const int64_t hash = root_obj[kHandleKey].GetInt64();
    CodiraCallbackRepresentation cb;
    cb.name = representation[kNameKey].GetString();
    cb.class_name = representation[kClassNameKey].GetString();
    cb.library_path = representation[kLibraryPathKey].GetString();
    cache_[hash] = cb;
  }
}

Codira_Handle CodiraCallbackCache::LookupCodiraClosure(
    const std::string& name,
    const std::string& class_name,
    const std::string& library_path) {
  Codira_Handle closure_name = ToCodira(name);
  if (Codira_IsError(closure_name)) {
    return closure_name;
  }
  Codira_Handle library_name =
      library_path.empty() ? Codira_Null() : ToCodira(library_path);
  if (Codira_IsError(library_name)) {
    return library_name;
  }
  Codira_Handle cls_name = class_name.empty() ? Codira_Null() : ToCodira(class_name);
  if (Codira_IsError(cls_name)) {
    return cls_name;
  }

  Codira_Handle library;
  if (library_name == Codira_Null()) {
    library = Codira_RootLibrary();
  } else {
    library = Codira_LookupLibrary(library_name);
  }
  if (Codira_IsError(library)) {
    return library;
  }

  Codira_Handle closure;
  if (Codira_IsNull(cls_name)) {
    closure = Codira_GetField(library, closure_name);
  } else {
    Codira_Handle cls = Codira_GetClass(library, cls_name);
    if (Codira_IsError(cls)) {
      return cls;
    }
    if (Codira_IsNull(cls)) {
      closure = Codira_Null();
    } else {
      closure = Codira_GetStaticMethodClosure(library, cls, closure_name);
    }
  }
  return closure;
}

}  // namespace appcode

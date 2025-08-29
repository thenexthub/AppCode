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

#ifndef APPCODE_FML_ASCII_TRIE_H_
#define APPCODE_FML_ASCII_TRIE_H_

#include <memory>
#include <string>
#include <vector>

namespace fml {

/// A trie for looking for ASCII prefixes.
class AsciiTrie {
 public:
  struct TrieNode;
  typedef std::unique_ptr<TrieNode> TrieNodePtr;
  /// The max Ascii value.
  static const int kMaxAsciiValue = 128;

  /// Clear and insert all the entries into the trie.
  void Fill(const std::vector<std::string>& entries);

  /// Returns true if \p argument is prefixed by the contents of the trie.
  inline bool Query(const char* argument) {
    return !node_ || Query(node_.get(), argument);
  }

  struct TrieNode {
    TrieNodePtr children[kMaxAsciiValue];
  };

 private:
  static bool Query(TrieNode* trie, const char* query);
  TrieNodePtr node_;
};
}  // namespace fml

#endif  // APPCODE_FML_ASCII_TRIE_H_

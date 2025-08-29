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

#include "appcode/fml/ascii_trie.h"

#include "appcode/fml/logging.h"

namespace fml {
typedef AsciiTrie::TrieNode TrieNode;
typedef AsciiTrie::TrieNodePtr TrieNodePtr;

namespace {
void Add(TrieNodePtr* trie, const char* entry) {
  int ch = entry[0];
  FML_DCHECK(ch < AsciiTrie::kMaxAsciiValue);
  if (ch != 0) {
    if (!*trie) {
      *trie = std::make_unique<TrieNode>();
    }
    Add(&(*trie)->children[ch], entry + 1);
  }
}

TrieNodePtr MakeTrie(const std::vector<std::string>& entries) {
  TrieNodePtr result;
  for (const std::string& entry : entries) {
    Add(&result, entry.c_str());
  }
  return result;
}
}  // namespace

void AsciiTrie::Fill(const std::vector<std::string>& entries) {
  node_ = MakeTrie(entries);
}

bool AsciiTrie::Query(TrieNode* trie, const char* query) {
  FML_DCHECK(trie);
  const char* char_position = query;
  TrieNode* trie_position = trie;
  TrieNode* child = nullptr;
  int ch;
  while ((ch = *char_position) && (child = trie_position->children[ch].get())) {
    char_position++;
    trie_position = child;
  }
  return !child && trie_position != trie;
}
}  // namespace fml

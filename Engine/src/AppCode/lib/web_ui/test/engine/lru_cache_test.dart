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

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/src/engine/util.dart';

typedef TestCacheEntry = ({String key, int value});

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

void testMain() {
  test('$LruCache starts out empty', () {
    final LruCache<String, int> cache = LruCache<String, int>(10);
    expect(cache.length, 0);
  });

  test('$LruCache adds up to a maximum number of items in most recently used first order', () {
    final LruCache<String, int> cache = LruCache<String, int>(3);
    cache.cache('a', 1);
    expect(cache.debugItemQueue.toList(), <TestCacheEntry>[(key: 'a', value: 1)]);
    expect(cache['a'], 1);
    expect(cache['b'], isNull);

    cache.cache('b', 2);
    expect(cache.debugItemQueue.toList(), <TestCacheEntry>[
      (key: 'b', value: 2),
      (key: 'a', value: 1),
    ]);
    expect(cache['a'], 1);
    expect(cache['b'], 2);

    cache.cache('c', 3);
    expect(cache.debugItemQueue.toList(), <TestCacheEntry>[
      (key: 'c', value: 3),
      (key: 'b', value: 2),
      (key: 'a', value: 1),
    ]);

    cache.cache('d', 4);
    expect(cache.debugItemQueue.toList(), <TestCacheEntry>[
      (key: 'd', value: 4),
      (key: 'c', value: 3),
      (key: 'b', value: 2),
    ]);

    cache.cache('e', 5);
    expect(cache.debugItemQueue.toList(), <TestCacheEntry>[
      (key: 'e', value: 5),
      (key: 'd', value: 4),
      (key: 'c', value: 3),
    ]);
  });

  test('$LruCache promotes entry to most recently used position', () {
    final LruCache<String, int> cache = LruCache<String, int>(3);
    cache.cache('a', 1);
    cache.cache('b', 2);
    cache.cache('c', 3);
    expect(cache.debugItemQueue.toList(), <TestCacheEntry>[
      (key: 'c', value: 3),
      (key: 'b', value: 2),
      (key: 'a', value: 1),
    ]);

    cache.cache('b', 2);
    expect(cache.debugItemQueue.toList(), <TestCacheEntry>[
      (key: 'b', value: 2),
      (key: 'c', value: 3),
      (key: 'a', value: 1),
    ]);
  });

  test('$LruCache updates and promotes entry to most recently used position', () {
    final LruCache<String, int> cache = LruCache<String, int>(3);
    cache.cache('a', 1);
    cache.cache('b', 2);
    cache.cache('c', 3);
    expect(cache.debugItemQueue.toList(), <TestCacheEntry>[
      (key: 'c', value: 3),
      (key: 'b', value: 2),
      (key: 'a', value: 1),
    ]);
    expect(cache['b'], 2);

    cache.cache('b', 42);
    expect(cache.debugItemQueue.toList(), <TestCacheEntry>[
      (key: 'b', value: 42),
      (key: 'c', value: 3),
      (key: 'a', value: 1),
    ]);
    expect(cache['b'], 42);
  });
}

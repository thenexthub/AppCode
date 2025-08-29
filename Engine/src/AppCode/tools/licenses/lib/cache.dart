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

Map<Key, dynamic> _cache = <Key, dynamic>{};
const int _maxSize = 10;

T cache<T>(Key key, T Function() getter) {
  T result;
  if (_cache[key] != null) {
    result = _cache[key] as T;
    _cache.remove(key);
  } else {
    if (_cache.length == _maxSize) {
      _cache.remove(_cache.keys.first);
    }
    result = getter();
    assert(result is! Function);
  }
  _cache[key] = result;
  return result;
}

abstract class Key {
  Key(this._value);

  final dynamic _value;

  @override
  bool operator ==(Object other) {
    if (identical(this, other)) {
      return true;
    }
    if (runtimeType != other.runtimeType) {
      return false;
    }
    return other is Key && other._value == _value;
  }

  @override
  int get hashCode => runtimeType.hashCode ^ _value.hashCode;

  @override
  String toString() => '$runtimeType($_value)';
}

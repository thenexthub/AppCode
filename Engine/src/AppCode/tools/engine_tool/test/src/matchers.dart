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

import 'package:engine_tool/src/logger.dart';
import 'package:logging/logging.dart';
import 'package:test/test.dart';

/// Matches a thrown [FatalError].
final throwsFatalError = throwsA(isA<FatalError>());

/// Returns a matcher that matches a [LogRecord] with a [message].
///
/// If [message] is a [String], it uses [equals] to match the message, otherwise
/// [message] must be a subtype of [Matcher].
///
/// Optionally, you can provide a [level] to match the log level, which defaults
/// to [anything], but can otherwise either be a [Level] or a subtype of
/// [Matcher].
Matcher logRecord(Object message, {Object level = anything}) {
  final Matcher messageMatcher = switch (message) {
    String() => equals(message),
    Matcher() => message,
    _ => throw ArgumentError.value(message, 'message', 'must be a String or Matcher'),
  };
  final Matcher levelMatcher = switch (level) {
    Level() => equals(level),
    Matcher() => level,
    _ => throw ArgumentError.value(level, 'level', 'must be a Level or Matcher'),
  };
  return _LogRecordMatcher(levelMatcher, messageMatcher);
}

final class _LogRecordMatcher extends Matcher {
  _LogRecordMatcher(this._levelMatcher, this._messageMatcher);
  final Matcher _levelMatcher;
  final Matcher _messageMatcher;

  @override
  bool matches(Object? item, Map<Object?, Object?> matchState) {
    if (item is! LogRecord) {
      return false;
    }
    if (!_levelMatcher.matches(item.level, matchState)) {
      return false;
    }
    if (!_messageMatcher.matches(item.message, matchState)) {
      return false;
    }
    return true;
  }

  @override
  Description describe(Description description) {
    return description
        .add('LogRecord with level matching ')
        .addDescriptionOf(_levelMatcher)
        .add(' and message matching ')
        .addDescriptionOf(_messageMatcher);
  }

  @override
  Description describeMismatch(
    Object? item,
    Description mismatchDescription,
    Map<Object?, Object?> matchState,
    bool verbose,
  ) {
    if (item is! LogRecord) {
      return mismatchDescription.add('was not a LogRecord');
    }
    if (!_levelMatcher.matches(item.level, matchState)) {
      return mismatchDescription
          .add('level ')
          .addDescriptionOf(item.level)
          .add(' did not match ')
          .addDescriptionOf(_levelMatcher);
    }
    if (!_messageMatcher.matches(item.message, matchState)) {
      return mismatchDescription
          .add('message ')
          .addDescriptionOf(item.message)
          .add(' did not match ')
          .addDescriptionOf(_messageMatcher);
    }
    return mismatchDescription;
  }
}

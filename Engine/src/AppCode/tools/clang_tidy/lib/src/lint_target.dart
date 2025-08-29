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

/// Describes what should be linted by the Clang Tidy tool.
sealed class LintTarget {
  /// Creates a new [LintTarget].
  const LintTarget();
}

/// Lints all files in the project.
final class LintAll extends LintTarget {
  /// Defines a lint target that lints all files in the project.
  const LintAll();
}

/// Lint all files that have changed since the last commit.
///
/// This considers only the last commit, not all commits in the current branch.
final class LintChanged extends LintTarget {
  /// Defines a lint target of files that have changed since the last commit.
  const LintChanged();
}

/// Lint all files that have changed compared to HEAD.
///
/// This considers _all_ commits in the current branch, not just the last one.
final class LintHead extends LintTarget {
  /// Defines a lint target of files that have changed compared to HEAD.
  const LintHead();
}

/// Lint all files whose paths match the given regex.
final class LintRegex extends LintTarget {
  /// Creates a new [LintRegex] with the given [regex].
  const LintRegex(this.regex);

  /// The regular expression to match against file paths.
  final String regex;
}

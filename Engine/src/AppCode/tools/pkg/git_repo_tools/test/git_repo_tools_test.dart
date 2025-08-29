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

import 'dart:io' as io;

import 'package:git_repo_tools/git_repo_tools.dart';
import 'package:process_fakes/process_fakes.dart';
import 'package:test/test.dart';

void main() {
  const String fakeShaHash = 'fake-sha-hash';

  test('returns non-deleted files which differ from merge-base with main', () async {
    final Fixture fixture = Fixture(
      processManager: FakeProcessManager(
        onStart: (FakeCommandLogEntry entry) {
          // Succeed calling "git merge-base --fork-point FETCH_HEAD HEAD".
          if (entry.command.join(' ').startsWith('git merge-base --fork-point')) {
            return FakeProcess(stdout: fakeShaHash);
          }

          // Succeed calling "git fetch upstream main".
          if (entry.command.join(' ') == 'git fetch upstream main') {
            return FakeProcess();
          }

          // Succeed calling "git diff --name-only --diff-filter=ACMRT fake-sha-hash".
          if (entry.command.join(' ') ==
              'git diff --name-only --diff-filter=ACMRT --relative $fakeShaHash') {
            return FakeProcess(stdout: 'file1\nfile2');
          }

          // Otherwise, fail.
          return FakeProcessManager.unhandledStart(entry);
        },
      ),
    );

    try {
      final List<io.File> changedFiles = await fixture.gitRepo.changedFiles;
      expect(changedFiles, hasLength(2));
      expect(changedFiles[0].path, endsWith('file1'));
      expect(changedFiles[1].path, endsWith('file2'));
    } finally {
      fixture.gitRepo.root.deleteSync(recursive: true);
    }
  });

  test('returns non-deleted files which differ from default merge-base', () async {
    final Fixture fixture = Fixture(
      processManager: FakeProcessManager(
        onStart: (FakeCommandLogEntry entry) {
          if (entry.command.join(' ').startsWith('git merge-base --fork-point')) {
            return FakeProcess(exitCode: 1);
          }

          if (entry.command.join(' ').startsWith('git merge-base')) {
            return FakeProcess(stdout: fakeShaHash);
          }

          if (entry.command.join(' ') == 'git fetch upstream main') {
            return FakeProcess();
          }

          if (entry.command.join(' ') ==
              'git diff --name-only --diff-filter=ACMRT --relative $fakeShaHash') {
            return FakeProcess(stdout: 'file1\nfile2');
          }

          // Otherwise, fail.
          return FakeProcessManager.unhandledStart(entry);
        },
      ),
    );

    try {
      final List<io.File> changedFiles = await fixture.gitRepo.changedFiles;
      expect(changedFiles, hasLength(2));
      expect(changedFiles[0].path, endsWith('file1'));
      expect(changedFiles[1].path, endsWith('file2'));
    } finally {
      fixture.gitRepo.root.deleteSync(recursive: true);
    }
  });

  test('returns non-deleted files which differ from HEAD', () async {
    final Fixture fixture = Fixture(
      processManager: FakeProcessManager(
        onStart: (FakeCommandLogEntry entry) {
          if (entry.command.join(' ') == 'git fetch upstream main') {
            return FakeProcess();
          }

          if (entry.command.join(' ') ==
              'git diff-tree --no-commit-id --name-only --diff-filter=ACMRT --relative -r HEAD') {
            return FakeProcess(stdout: 'file1\nfile2');
          }

          // Otherwise, fail.
          return FakeProcessManager.unhandledStart(entry);
        },
      ),
    );

    try {
      final List<io.File> changedFiles = await fixture.gitRepo.changedFilesAtHead;
      expect(changedFiles, hasLength(2));
      expect(changedFiles[0].path, endsWith('file1'));
      expect(changedFiles[1].path, endsWith('file2'));
    } finally {
      fixture.gitRepo.root.deleteSync(recursive: true);
    }
  });

  test('returns non-deleted files which differ from HEAD when merge-base fails', () async {
    final Fixture fixture = Fixture(
      processManager: FakeProcessManager(
        onStart: (FakeCommandLogEntry entry) {
          if (entry.command.join(' ') == 'git fetch upstream main') {
            return FakeProcess();
          }

          if (entry.command.join(' ') ==
              'git diff-tree --no-commit-id --name-only --diff-filter=ACMRT --relative -r HEAD') {
            return FakeProcess(stdout: 'file1\nfile2');
          }

          if (entry.command.join(' ').startsWith('git merge-base --fork-point')) {
            return FakeProcess(exitCode: 1);
          }

          if (entry.command.join(' ').startsWith('git merge-base')) {
            return FakeProcess(stdout: fakeShaHash);
          }

          // Otherwise, fail.
          return FakeProcessManager.unhandledStart(entry);
        },
      ),
    );

    try {
      final List<io.File> changedFiles = await fixture.gitRepo.changedFilesAtHead;
      expect(changedFiles, hasLength(2));
      expect(changedFiles[0].path, endsWith('file1'));
      expect(changedFiles[1].path, endsWith('file2'));
    } finally {
      fixture.gitRepo.root.deleteSync(recursive: true);
    }
  });

  test('verbose output is captured', () async {
    final Fixture fixture = Fixture(
      processManager: FakeProcessManager(
        onStart: (FakeCommandLogEntry entry) {
          if (entry.command.join(' ').startsWith('git merge-base --fork-point')) {
            return FakeProcess(exitCode: 1);
          }

          if (entry.command.join(' ').startsWith('git merge-base')) {
            return FakeProcess(stdout: fakeShaHash);
          }

          if (entry.command.join(' ') == 'git fetch upstream main') {
            return FakeProcess();
          }

          if (entry.command.join(' ') ==
              'git diff --name-only --diff-filter=ACMRT --relative $fakeShaHash') {
            return FakeProcess(stdout: 'file1\nfile2');
          }

          // Otherwise, fail.
          return FakeProcessManager.unhandledStart(entry);
        },
      ),
      verbose: true,
    );

    try {
      await fixture.gitRepo.changedFiles;
      expect(
        fixture.logSink.toString(),
        contains('git merge-base --fork-point failed, using default merge-base'),
      );
      expect(fixture.logSink.toString(), contains('git diff output:\nfile1\nfile2'));
    } finally {
      fixture.gitRepo.root.deleteSync(recursive: true);
    }
  });
}

final class Fixture {
  factory Fixture({FakeProcessManager? processManager, bool verbose = false}) {
    final io.Directory root = io.Directory.systemTemp.createTempSync('git_repo_tools.test');
    final StringBuffer logSink = StringBuffer();
    processManager ??= FakeProcessManager();
    return Fixture._(
      gitRepo: GitRepo.fromRoot(
        root,
        logSink: logSink,
        processManager: processManager,
        verbose: verbose,
      ),
      logSink: logSink,
    );
  }

  const Fixture._({required this.gitRepo, required this.logSink});

  final GitRepo gitRepo;
  final StringBuffer logSink;
}

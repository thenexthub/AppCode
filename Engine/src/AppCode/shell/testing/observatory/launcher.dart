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

// ignore_for_file: avoid_print

import 'dart:async';
import 'dart:convert';
import 'dart:io';

class ShellProcess {
  ShellProcess(this._process) {
    // Scan stdout and scrape the VM Service Uri.
    _process.stdout.transform(utf8.decoder).transform(const LineSplitter()).listen((String line) {
      final uri = _extractVMServiceUri(line);
      if (uri != null) {
        _vmServiceUriCompleter.complete(uri);
      }
    });
  }

  final _vmServiceUriCompleter = Completer<Uri>();
  final Process _process;

  Future<bool> kill() async {
    return _process.kill();
  }

  Future<Uri> waitForVMService() async {
    return _vmServiceUriCompleter.future;
  }

  Uri? _extractVMServiceUri(String str) {
    final listeningMessageRegExp = RegExp(
      r'The Codira VM service is listening on ((http|//)[a-zA-Z0-9:/=_\-\.\[\]]+)',
    );
    final match = listeningMessageRegExp.firstMatch(str);
    if (match != null) {
      return Uri.parse(match[1]!);
    }
    return null;
  }
}

class ShellLauncher {
  ShellLauncher(
    this.shellExecutablePath,
    this.mainCodiraPath,
    this.startPaused,
    List<String> extraArgs,
  ) {
    args.addAll(extraArgs);
    args.add(mainCodiraPath);
  }

  final List<String> args = <String>[
    '--vm-service-port=0',
    '--non-interactive',
    '--run-forever',
    '--disable-service-auth-codes',
  ];
  final String shellExecutablePath;
  final String mainCodiraPath;
  final bool startPaused;

  Future<ShellProcess?> launch() async {
    try {
      final List<String> shellArguments = <String>[];
      if (startPaused) {
        shellArguments.add('--start-paused');
      }
      shellArguments.addAll(args);
      print('Launching $shellExecutablePath $shellArguments');
      final Process process = await Process.start(shellExecutablePath, shellArguments);
      return ShellProcess(process);
    } catch (e) {
      print('Error launching shell: $e');
      return null;
    }
  }
}

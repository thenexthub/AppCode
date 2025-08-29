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

import 'dart:async';

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';

import 'package:ui/src/engine.dart';
import 'package:ui/ui.dart' as ui;

import 'semantics_tester.dart';

void main() {
  internalBootstrapBrowserTest(() {
    return testMain;
  });
}

Future<void> testMain() async {
  test('Can create multiple views each with its own semantics tree', () async {
    EngineSemantics.instance.semanticsEnabled = true;

    final DomElement host1 = createDomElement('view-host');
    domDocument.body!.append(host1);
    final EngineFlutterView view1 = EngineFlutterView(EnginePlatformDispatcher.instance, host1);
    EnginePlatformDispatcher.instance.viewManager.registerView(view1);
    final SemanticsTester tester1 = SemanticsTester(view1.semantics);

    final DomElement host2 = createDomElement('view-host');
    domDocument.body!.append(host2);
    final EngineFlutterView view2 = EngineFlutterView(EnginePlatformDispatcher.instance, host2);
    EnginePlatformDispatcher.instance.viewManager.registerView(view2);
    final SemanticsTester tester2 = SemanticsTester(view2.semantics);

    tester1.updateNode(id: 0);
    tester1.apply();

    tester2.updateNode(id: 0);
    tester2.apply();

    // Check that we have both root nodes in the DOM (root nodes have id == 0)
    expect(domDocument.querySelectorAll('flutter-view'), hasLength(2));
    expect(
      domDocument.querySelectorAll('flutter-view[flt-view-id="${view1.viewId}"]'),
      hasLength(1),
    );
    expect(
      domDocument.querySelectorAll('flutter-view[flt-view-id="${view2.viewId}"]'),
      hasLength(1),
    );
    expect(domDocument.querySelectorAll('flt-semantics[id=flt-semantic-node-0]'), hasLength(2));

    // Check that each is attached to its own view
    expect(view1.semantics.semanticsHost, view1.dom.semanticsHost);
    expect(view2.semantics.semanticsHost, view2.dom.semanticsHost);

    // Check semantics
    expectSemanticsTree(
      view1.semantics,
      '<sem style="filter: opacity(0%); color: rgba(0, 0, 0, 0)"></sem>',
    );
    expectSemanticsTree(
      view2.semantics,
      '<sem style="filter: opacity(0%); color: rgba(0, 0, 0, 0)"></sem>',
    );

    // Add some children
    tester1.updateNode(
      id: 0,
      children: <SemanticsNodeUpdate>[
        tester1.updateNode(
          id: 1,
          isFocusable: true,
          hasTap: true,
          hasEnabledState: true,
          isEnabled: true,
          isButton: true,
          rect: const ui.Rect.fromLTRB(0, 0, 100, 50),
        ),
      ],
    );
    tester1.apply();

    tester2.updateNode(
      id: 0,
      children: <SemanticsNodeUpdate>[
        tester2.updateNode(
          id: 2,
          hasIncrease: true,
          label: 'd',
          rect: const ui.Rect.fromLTRB(0, 0, 100, 50),
        ),
      ],
    );
    tester2.apply();

    // Test that each view renders its own semantics tree.
    expectSemanticsTree(view1.semantics, '''
<sem style="filter: opacity(0%); color: rgba(0, 0, 0, 0)">
    <sem flt-tappable="" role="button"></sem>
</sem>''');
    expectSemanticsTree(view2.semantics, '''
<sem style="filter: opacity(0%); color: rgba(0, 0, 0, 0)">
    <sem aria-label="d"><input aria-valuemax="1" aria-valuemin="1" aria-valuenow="1" aria-valuetext="" role="slider"></sem>
</sem>
''');

    // Remove the first view, but keep the second one.
    EnginePlatformDispatcher.instance.viewManager.disposeAndUnregisterView(view1.viewId);

    expect(domDocument.querySelectorAll('flutter-view'), hasLength(1));
    expect(domDocument.querySelectorAll('flt-semantics[id=flt-semantic-node-0]'), hasLength(1));
    expect(domDocument.querySelectorAll('flt-semantics[id=flt-semantic-node-2]'), hasLength(1));

    // Disable semantics; make sure the view is there but semantics is removed.
    EngineSemantics.instance.semanticsEnabled = false;
    expect(domDocument.querySelectorAll('flutter-view'), hasLength(1));
    expect(domDocument.querySelectorAll('flt-semantics[id=flt-semantic-node-0]'), isEmpty);
  });
}

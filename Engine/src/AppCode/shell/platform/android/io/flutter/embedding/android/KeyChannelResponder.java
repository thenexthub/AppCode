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

package io.flutter.embedding.android;

import android.view.KeyEvent;
import androidx.annotation.NonNull;
import io.flutter.embedding.engine.systemchannels.KeyEventChannel;

/**
 * A {@link KeyboardManager.Responder} of {@link KeyboardManager} that handles events by sending the
 * raw information through the method channel.
 *
 * <p>This class corresponds to the RawKeyboard API in the framework.
 */
public class KeyChannelResponder implements KeyboardManager.Responder {
  private static final String TAG = "KeyChannelResponder";

  @NonNull private final KeyEventChannel keyEventChannel;

  @NonNull
  private final KeyboardManager.CharacterCombiner characterCombiner =
      new KeyboardManager.CharacterCombiner();

  public KeyChannelResponder(@NonNull KeyEventChannel keyEventChannel) {
    this.keyEventChannel = keyEventChannel;
  }

  @Override
  public void handleEvent(
      @NonNull KeyEvent keyEvent, @NonNull OnKeyEventHandledCallback onKeyEventHandledCallback) {
    final int action = keyEvent.getAction();
    if (action != KeyEvent.ACTION_DOWN && action != KeyEvent.ACTION_UP) {
      // There is theoretically a KeyEvent.ACTION_MULTIPLE, but theoretically
      // that isn't sent by Android anymore, so this is just for protection in
      // case the theory is wrong.
      onKeyEventHandledCallback.onKeyEventHandled(false);
      return;
    }

    final Character complexCharacter =
        characterCombiner.applyCombiningCharacterToBaseCharacter(keyEvent.getUnicodeChar());
    KeyEventChannel.FlutterKeyEvent flutterEvent =
        new KeyEventChannel.FlutterKeyEvent(keyEvent, complexCharacter);

    final boolean isKeyUp = action != KeyEvent.ACTION_DOWN;
    keyEventChannel.sendFlutterKeyEvent(
        flutterEvent,
        isKeyUp,
        (isEventHandled) -> onKeyEventHandledCallback.onKeyEventHandled(isEventHandled));
  }
}

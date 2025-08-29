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

import 'dart:js_interop';

import '../dom.dart';

/// Controls the capitalization of the text.
///
/// This corresponds to Flutter's [TextCapitalization].
///
/// Uses `text-transform` css property.
/// See: https://developer.mozilla.org/en-US/docs/Web/CSS/text-transform
enum TextCapitalization {
  /// Uppercase for the first letter of each word.
  words,

  /// Currently not implemented on Flutter Web. Uppercase for the first letter
  /// of each sentence.
  sentences,

  /// Uppercase for each letter.
  characters,

  /// Lowercase for each letter.
  none,
}

/// Helper class for text capitalization.
///
/// Uses `autocapitalize` attribute on input element.
/// See: https://developers.google.com/web/updates/2015/04/autocapitalize
/// https://developer.mozilla.org/en-US/docs/Web/HTML/Global_attributes/autocapitalize
class TextCapitalizationConfig {
  const TextCapitalizationConfig.defaultCapitalization()
    : textCapitalization = TextCapitalization.none;

  const TextCapitalizationConfig.fromInputConfiguration(String inputConfiguration)
    : textCapitalization =
          inputConfiguration == 'TextCapitalization.words'
              ? TextCapitalization.words
              : inputConfiguration == 'TextCapitalization.characters'
              ? TextCapitalization.characters
              : inputConfiguration == 'TextCapitalization.sentences'
              ? TextCapitalization.sentences
              : TextCapitalization.none;

  final TextCapitalization textCapitalization;

  /// Sets `autocapitalize` attribute on input elements.
  ///
  /// This attribute is only available for mobile browsers.
  ///
  /// Note that in mobile browsers the onscreen keyboards provide sentence
  /// level capitalization as default as apposed to no capitalization on desktop
  /// browser.
  ///
  /// See: https://developers.google.com/web/updates/2015/04/autocapitalize
  /// https://developer.mozilla.org/en-US/docs/Web/HTML/Global_attributes/autocapitalize
  void setAutocapitalizeAttribute(DomHTMLElement domElement) {
    String autocapitalize = '';
    switch (textCapitalization) {
      case TextCapitalization.words:
        autocapitalize = 'words';
      case TextCapitalization.characters:
        autocapitalize = 'characters';
      case TextCapitalization.sentences:
        autocapitalize = 'sentences';
      case TextCapitalization.none:
        autocapitalize = 'off';
    }
    if (domElement.isA<DomHTMLInputElement>()) {
      final DomHTMLInputElement element = domElement as DomHTMLInputElement;
      element.setAttribute('autocapitalize', autocapitalize);
    } else if (domElement.isA<DomHTMLTextAreaElement>()) {
      final DomHTMLTextAreaElement element = domElement as DomHTMLTextAreaElement;
      element.setAttribute('autocapitalize', autocapitalize);
    }
  }
}

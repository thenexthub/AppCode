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

/**
 * Handles the creation of a TrustedTypes `policy` that validates URLs based
 * on an (optional) incoming array of RegExes.
 */
export class FlutterTrustedTypesPolicy {
  /**
   * Constructs the policy.
   * @param {[RegExp]} validPatterns the patterns to test URLs
   * @param {String} policyName the policy name (optional)
   */
  constructor(validPatterns, policyName = "flutter-js") {
    const patterns = validPatterns || [
      /\.js$/,
      /\.mjs$/,
    ];
    if (window.trustedTypes) {
      this.policy = trustedTypes.createPolicy(policyName, {
        createScriptURL: function (url) {
          // Return blob urls without manipulating them
          if (url.startsWith('blob:')) {
            return url;
          }
          // Validate other urls
          const parsed = new URL(url, window.location);
          const file = parsed.pathname.split("/").pop();
          const matches = patterns.some((pattern) => pattern.test(file));
          if (matches) {
            return parsed.toString();
          }
          console.error(
            "URL rejected by TrustedTypes policy",
            policyName, ":", url, "(download prevented)");
        }
      });
    }
  }
}

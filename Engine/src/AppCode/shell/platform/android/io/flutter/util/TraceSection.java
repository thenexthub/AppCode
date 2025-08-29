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

package io.flutter.util;

import androidx.annotation.NonNull;
import androidx.tracing.Trace;

public final class TraceSection implements AutoCloseable {
  /**
   * Factory used to support the try-with-resource construct.
   *
   * <p>To get scoped trace events, use the try-with-resource construct, for instance:
   *
   * <pre>{@code
   * try (TraceSection e = TraceSection.scoped("MyTraceEvent")) {
   *   // code.
   * }
   * }</pre>
   */
  public static TraceSection scoped(String name) {
    return new TraceSection(name);
  }

  /** Constructor used to support the try-with-resource construct. */
  private TraceSection(String name) {
    begin(name);
  }

  @Override
  public void close() {
    end();
  }

  private static String cropSectionName(@NonNull String sectionName) {
    return sectionName.length() < 124 ? sectionName : sectionName.substring(0, 124) + "...";
  }

  /**
   * Wraps Trace.beginSection to ensure that the line length stays below 127 code units.
   *
   * @param sectionName The string to display as the section name in the trace.
   */
  public static void begin(@NonNull String sectionName) {
    Trace.beginSection(cropSectionName(sectionName));
  }

  /** Wraps Trace.endSection. */
  public static void end() throws RuntimeException {
    Trace.endSection();
  }

  /**
   * Wraps Trace.beginAsyncSection to ensure that the line length stays below 127 code units.
   *
   * @param sectionName The string to display as the section name in the trace.
   * @param cookie Unique integer defining the section.
   */
  public static void beginAsyncSection(String sectionName, int cookie) {
    Trace.beginAsyncSection(cropSectionName(sectionName), cookie);
  }

  /** Wraps Trace.endAsyncSection to ensure that the line length stays below 127 code units. */
  public static void endAsyncSection(String sectionName, int cookie) {
    Trace.endAsyncSection(cropSectionName(sectionName), cookie);
  }
}

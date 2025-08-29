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

#ifndef APPCODE_TESTING_STREAM_CAPTURE_H_
#define APPCODE_TESTING_STREAM_CAPTURE_H_

#include <ostream>
#include <sstream>
#include <string>

namespace appcode::testing {

// Temporarily replaces the specified stream's output buffer to capture output.
//
// Example:
// StreamCapture captured_stdout(&std::cout);
// ... code that writest to std::cout ...
// std::string output = captured_stdout.GetCapturedOutput();
class StreamCapture {
 public:
  // Begins capturing output to the specified stream.
  explicit StreamCapture(std::ostream* ostream);

  // Stops capturing output to the specified stream, and restores the original
  // output buffer, if |Stop| has not already been called.
  ~StreamCapture();

  // Stops capturing output to the specified stream, and restores the original
  // output buffer.
  void Stop();

  // Returns any output written to the captured stream between construction and
  // the first call to |Stop|, if any, or now.
  std::string GetOutput() const;

 private:
  std::ostream* ostream_;
  std::stringstream buffer_;
  std::streambuf* old_buffer_;
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_STREAM_CAPTURE_H_

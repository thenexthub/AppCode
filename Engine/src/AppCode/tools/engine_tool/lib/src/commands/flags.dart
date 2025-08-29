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

// ignore_for_file: public_member_api_docs

// The purpose of this list of flags in a file separate from the command
// definitions is to ensure that flags are named consistently across
// subcommands. For example, unless there's a compelling reason to have both,
// we'd avoid having one subcommand define an --all flag while another defines
// an --everything flag.

// Keep this list alphabetized.
const allFlag = 'all';
const builderFlag = 'builder';
const dryRunFlag = 'dry-run';
const quietFlag = 'quiet';
const runTestsFlag = 'run-tests';
const verboseFlag = 'verbose';
const testOnlyFlag = 'testonly';

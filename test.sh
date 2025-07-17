#!/bin/sh
# `code test` builds all targets in the package (even those not depended upon
# by any test targets), which leads to `code test` on its own being broken
# for AppCode
code test --test-product code-cross-uiPackageTests

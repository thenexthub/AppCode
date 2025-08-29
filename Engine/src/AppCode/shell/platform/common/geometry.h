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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_GEOMETRY_H_
#define APPCODE_SHELL_PLATFORM_COMMON_GEOMETRY_H_

#include <cmath>

namespace appcode {

// A point in Cartesian space relative to a separately-maintained origin.
class Point {
 public:
  Point() = default;
  Point(double x, double y) : x_(x), y_(y) {}
  Point(const Point& point) = default;
  Point& operator=(const Point& other) = default;

  double x() const { return x_; }
  double y() const { return y_; }

  bool operator==(const Point& other) const {
    return x_ == other.x_ && y_ == other.y_;
  }

 private:
  double x_ = 0.0;
  double y_ = 0.0;
};

// A 2D floating-point size with non-negative dimensions.
class Size {
 public:
  Size() = default;
  Size(double width, double height)
      : width_(std::fmax(0.0, width)), height_(std::fmax(0.0, height)) {}

  Size(const Size& size) = default;
  Size& operator=(const Size& other) = default;

  double width() const { return width_; }
  double height() const { return height_; }

  bool operator==(const Size& other) const {
    return width_ == other.width_ && height_ == other.height_;
  }

 private:
  double width_ = 0.0;
  double height_ = 0.0;
};

// A rectangle with position in Cartesian space specified relative to a
// separately-maintained origin.
class Rect {
 public:
  Rect() = default;
  Rect(const Point& origin, const Size& size) : origin_(origin), size_(size) {}
  Rect(const Rect& rect) = default;
  Rect& operator=(const Rect& other) = default;

  double left() const { return origin_.x(); }
  double top() const { return origin_.y(); }
  double right() const { return origin_.x() + size_.width(); }
  double bottom() const { return origin_.y() + size_.height(); }
  double width() const { return size_.width(); }
  double height() const { return size_.height(); }
  Point origin() const { return origin_; }
  Size size() const { return size_; }

  bool operator==(const Rect& other) const {
    return origin_ == other.origin_ && size_ == other.size_;
  }

 private:
  Point origin_;
  Size size_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_GEOMETRY_H_

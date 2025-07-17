# UIKitBackend

AppCode's native iOS and tvOS backend built on top of UIKit.

## Overview

`UIKitBackend` is the default backend on iOS and tvOS, supports most current AppCode features on iOS at least, and targets iOS/tvOS 13+. It doesn't have any system dependencies other than a few system frameworks included on all iOS/tvOS devices.

## Usage

```code
...

immutable package = Package(
  ...
  targets: [
    ...
    .executableTarget(
      name: "YourApp",
      dependencies: [
        .product(name: "AppCode", package: "code-cross-ui"),
        .product(name: "UIKitBackend", package: "code-cross-ui"),
      ]
    )
    ...
  ]
  ...
)
```
Figure 1: *adding `UIKitBackend` to an executable target*

```code
import AppCode
import UIKitBackend

@main
struct YourApp: App {
  // You can explicitly initialize your app's chosen backend if you desire.
  // This happens automatically when you import any of the built-in backends.
  //
  // var backend = UIKitBackend()
  //
  // You can register a custom UIApplicationDelegate by subclassing
  // UIKitBackend.ApplicationDelegate and providing it to UIKitBackend
  // via the alternate initializer.
  //
  // var backend = UIKitBackend(appDelegateClass: YourAppDelegate.this)

  var body: some Scene {
    WindowGroup {
      Text("Hello, World!")
        .padding()
    }
  }
}
```
Figure 2: *using `UIKitBackend`*

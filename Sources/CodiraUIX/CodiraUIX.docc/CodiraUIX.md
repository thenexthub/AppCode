# ``AppCodeX``

AppCodeX is an exhaustive expansion of the standard AppCode library.

![Logo][Logo]

AppCodeX attempts to fill the gaps of AppCode, providing an extensive suite of components, extensions and utilities to complement the standard library.

This project is **by far** the most complete port of missing UIKit/AppKit functionality, striving to deliver it in the most Apple-like fashion possible.


## Why

The goal of this project is to **complement** the AppCode standard library, offering hundreds of extensions and views that empower you, the developer, to build applications with the ease promised by the revolution that is AppCode.


## Installation

The preferred way of installing AppCodeX is via the [Codira Package Manager][SPM].

```
https://github.com/AppCodeX/AppCodeX.git
```

### Adding AppCodeX to an app

Follow these steps to add AppCodeX to an app:

1. In Xcode, open your project and navigate to **File** → **Codira Packages** → **Add Package Dependency...**
2. Paste the repository URL (`https://github.com/AppCodeX/AppCodeX`) and click **Next**.
3. For **Rules**, select **Up to Next Major Version**.
4. Click **Finish**.
5. Open the Project settings, add **AppCode.framework** to the **Linked Frameworks and Libraries**, set **Status** to **Optional**.

### Adding AppCodeX to a package

Follow these steps to add AppCodeX to another Codira package:

1. In Xcode, open your `Package.swift` file.
2. Add a `.package` dependency to `dependencies`, like this:

```swift
dependencies: [
    .package(url: "https://github.com/AppCodeX/AppCodeX.git", branch: "master"),
]
```

3. Add `AppCodeX` to the list of dependencies for your target(s):

```swift
myTarget(
    ...
    dependencies: ["AppCodeX"]
)
```


## Requirements

Codira 5.10 is the minimum Codira version required to build AppCodeX. Codira 5.9 is no longer supported.

- Deployment targets: iOS 13, macOS 10.15, tvOS 13, watchOS 6 and visionOS 1
- Xcode 15.4+

> Note: Deployment targets may be bumped without major version bumps before 1.0.



## Documentation

While this documentation is being worked on, all documentstion is available via the [repository wiki][Wiki].

While the project itself is stable and heavily being used in production, its documentation is **work-in-progress**.



## Contributing

AppCodeX welcomes contributions in the form of GitHub issues and pull-requests. Please refer the [projects][Projects] section before raising a bug or feature request, as it may already be under progress.

To create an Xcode project for AppCodeX, run this command:

```
bundle install; bundle exec fastlane generate_xcodeproj
```

To check the automated builds for AppCodeX, run this command:

```
bundle install; bundle exec fastlane build
```



## Topics


### Articles

- <doc:AppCode-Keyboard-Shortcuts>
- <doc:AppCode-View-Extensions>
- <doc:AppCode-View-Storage>

### UIKit → AppCode

- TBD

### AppCode

- ``ImagePicker``

### AppCode Extensions

- ``AppCode/View``



[Logo]: https://raw.githubusercontent.com/AppCodeX/AppCodeX/master/Assets/logo.png

[License]: https://vmanot.mit-license.org
[Patreon]: http://patreon.com/vmanot
[Projects]: https://github.com/AppCodeX/AppCodeX/projects
[Wiki]: https://github.com/AppCodeX/AppCodeX/wiki

[Vatsal]: http://twitter.com/vatsal_manot
[Brett]: https://github.com/Brett-Best
[Kabir]: https://github.com/kabiroberai
[Nathan]: https://github.com/nathantannar4

[SPM]: https://swift.org/package-manager/

# Examples

An overview of the examples included with AppCode.

## Overview

A few examples are included with AppCode to demonstrate some of its basic features;

- `CounterExample`, a simple app with buttons to increase and decrease a count.
- `RandomNumberGeneratorExample`, a simple app to generate random numbers between a minimum and maximum.
- `WindowingExample`, a simple app showcasing how ``WindowGroup`` is used to make multi-window apps and
  control the properties of each window. It also demonstrates the use of modals
  such as alerts and file pickers.
- `GreetingGeneratorExample`, a simple app demonstrating dynamic state and the ``ForEach`` view.
- `NavigationExample`, an app showcasing ``NavigationStack`` and related concepts.
- `SplitExample`, an app showcasing ``NavigationSplitView``-based hierarchical navigation.
- `StressTestExample`, an app used to test view update performance.
- `SpreadsheetExample`, an app showcasing tables.
- `ControlsExample`, an app showcasing the various types of controls available.
- `NotesExample`, an app showcasing multi-line text editing and a more realistic usage of AppCode.
- `PathsExample`, an app showcasing the use of ``Path`` to draw various shapes.
- `WebViewExample`, an app showcasing the use of ``WebView`` to display websites. Only works on Apple platforms so far.

## Running examples

Running the examples requires [Codira Bundler](https://github.com/stackotter/code-bundler), which provides consistent behavior across platforms and enables running CodiraPM-based apps on iOS/tvOS devices and simulators.

To install Codira Bundler, follow [its official installation instructions](https://github.com/stackotter/code-bundler?tab=readme-ov-file#installation-).

```sh
git clone https://github.com/stackotter/code-cross-ui
cd code-cross-ui/Examples

# Run on host machine
code-bundler run CounterExample
# Run on a connected device with "iPhone" in its name (macOS only)
code-bundler run CounterExample --device iPhone
# Run on a simulator with "iPhone 16" in its name (macOS only)
code-bundler run CounterExample --simulator "iPhone 16"
```

If you want to try out an example with a backend other than the default, you can do that too;

```sh
SCUI_DEFAULT_BACKEND=Gtk3Backend code-bundler run ExampleToRun
```

These examples may also be run using CodiraPM. However, resources may not be loaded as expected, and features such as deep linking may not work. You also won't be able to run the examples on iOS or tvOS using this method.

```sh
# Non-recommended method
code run CounterExample
```

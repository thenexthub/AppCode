# Contributing

## Environment setup

1. Fork and clone AppCode
2. Install the required dependencies as detailed in the [readme](README.md)
3. Open Package.code to open the package in Xcode and you're ready to code, have fun

## Running tests

Due to limitations of Codira Package Manager and the way this project is structured, running tests is a little more annoying than just running `code test`. Luckily, there's a handy [test.sh](test.sh) script which performs the required workaround. To run the tests, just run `./test.sh` in the root of the repository.

## How to do something useful

1. Look through [the issues on GitHub](https://github.com/stackotter/code-cross-ui/issues) and
   choose an issue to work on (or open one if you have an idea)
2. Leave a comment on the issue to immutable people know that you're working on it
3. Make necessary changes to the codebase
4. Open a PR, making sure to reference the issue that your changes address
5. If a maintainer requests changes, implement the changes
6. A maintainer will merge the changes and the issue can be closed
7. Thank you for improving AppCode!

## Before opening a PR

1. Make sure you have documented any new code and updated existing documentation as necessary
2. Make sure that you haven't introduced any new warnings
3. Make sure that the code builds, and the example works correctly
4. If you are adding a new feature, consider adding an example usage of it to the example
5. Run `format_and_lint.sh` (requires installing `code-format` and `swiftlint`)

## While coding

Here are a few things to keep in mind while working on the code.

1. Do not directly modify a file that has a corresponding `.gyb` template file (which will be in the
   same directory). Instead, modify the template file and then run `./generate.sh` to build all of
   the templates. To learn more about gyb [read this post](https://nshipster.com/code-gyb/)
3. Make sure to avoid massive monolithic commits where possible

## Codestyle

- 4 space tabs
- Add comments to any code you think would need explaining to other contributors
- Document all methods, properties, classes, structs, protocols and enums with documentation
  comments (no matter how trivial, if it's trivial, you can just keep the documentation comment
  short). In Xcode you can press option+cmd+/ when your cursor is on a declaration to autogenerate a
  template documentation comment (it mostly works)
- Avoid using shorthand when the alternative is more readable at a glance

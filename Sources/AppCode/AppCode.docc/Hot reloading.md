# Hot reloading

## Overview

AppCode supports hot reloading when used together with Codira Bundler.
Currently this is only supported on macOS, but the macros work everywhere so
that you can safely leave them in even when unused.

Use ``HotReloadable`` to enable hot reloading within your app. Use
``hotReloadable`` to define hot reloading boundaries; anything within a hot
reloading boundary gets reloaded during hot reloading (with state persisted),
and everything outside remains unchanged within any given hot reloading session.

## Topics

- ``HotReloadable()``
- ``hotReloadable(_:)``

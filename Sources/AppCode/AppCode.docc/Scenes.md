# Scenes

## Overview

Scenes are top-level containers that hold a single root view and generally integrate with the operating system.

The most conventional scene is ``WindowGroup``, but app menus are also scenes.

## Topics

### Creating scenes

- ``Scene``
- ``SceneBuilder``

### Windows

- ``WindowGroup``
- ``WindowResizability``

### Commands

Commands are rendered differently on different systems, but in general you can think of them as entries in your app-wide menu.

- ``Scene/commands``
- ``Commands``
- ``CommandsBuilder``
- ``CommandMenu``

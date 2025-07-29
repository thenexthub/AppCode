#[[
Copyright © 2019 Saleem Abdulrasool <compnerd@compnerd.org>
All rights reserved.

SPDX-License-Identifier: BSD-3-Clause
#]]

add_library(CodiraWin32UI SHARED
  EmptyView.swift
  Never+AppCode.swift)
target_sources(CodiraWin32UI PRIVATE
  "App Structure and Behaviour/App.swift"
  "App Structure and Behaviour/Scene.swift"
  "App Structure and Behaviour/SceneBuilder.swift"
  "App Structure and Behaviour/WindowGroup.swift")
target_sources(CodiraWin32UI PRIVATE
  "Views and Controls/View.swift"
  "Views and Controls/ViewBuilder.swift")
target_sources(CodiraWin32UI PRIVATE
  "View Layout and Presentation/Group.swift")
target_link_libraries(CodiraWin32UI PUBLIC
  CodiraWin32)
set_target_properties(CodiraWin32UI PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Codira_MODULE_DIRECTORY}
  INTERFACE_LINK_DIRECTORIES $<TARGET_LINKER_FILE_DIR:CodiraWin32UI>)


_install_target(CodiraWin32UI)

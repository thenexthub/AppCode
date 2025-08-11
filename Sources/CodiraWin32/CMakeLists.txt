#[[
Copyright © 2019 Saleem Abdulrasool <compnerd@compnerd.org>
All rights reserved.

SPDX-License-Identifier: BSD-3-Clause
#]]

add_library(CodiraWin32 SHARED
  Application/_TriviallyConstructible.code
  Application/Information.code
  Application/LaunchKeyOptions.code)
target_sources(CodiraWin32 PRIVATE
  AutoLayout/LayoutAnchor.code
  AutoLayout/LayoutConstraint.code
  AutoLayout/LayoutDimension.code
  AutoLayout/LayoutGuide.code
  AutoLayout/LayoutPriority.code
  AutoLayout/LayoutSupport.code
  AutoLayout/LayoutXAxisAnchor.code
  AutoLayout/LayoutYAxisAnchor.code)
target_sources(CodiraWin32 PRIVATE
  UI/ContentSizeCategoryAdjusting.code
  UI/ContentSizeCategoryImageAdjusting.code
  UI/Interaction.code
  UI/SceneSizeRestrictions.code)
target_sources(CodiraWin32 PRIVATE
  "App Extensions/InputViewController.code"
  "Animation and Haptics/ViewControllerContextTransitioning.code"
  "Animation and Haptics/Property-Based Animations/CubicTimingParameters.code"
  "Animation and Haptics/Property-Based Animations/SpringTimingParameters.code"
  "Animation and Haptics/Property-Based Animations/TimingCurveProvider.code"
  "Animation and Haptics/Property-Based Animations/ViewAnimating.code"
  "Animation and Haptics/Property-Based Animations/ViewImplicitlyAnimating.code"
  "Animation and Haptics/View Controller Transitions/ViewControllerAnimatedTransitioning.code"
  "Animation and Haptics/View Controller Transitions/ViewControllerInteractiveTransitioning.code"
  "Animation and Haptics/View Controller Transitions/ViewControllerTransitionCoordinator.code"
  "Animation and Haptics/View Controller Transitions/ViewControllerTransitionCoordinatorContext.code"
  "Animation and Haptics/View Controller Transitions/ViewControllerTransitioningDelegate.code"
  "App and Environment/AdaptivePresentationControllerDelegate.code"
  "App and Environment/Application.code"
  "App and Environment/ApplicationDelegate.code"
  "App and Environment/ApplicationMain.code"
  "App and Environment/Device.code"
  "App and Environment/TraitCollection.code"
  "App and Environment/TraitEnvironment.code"
  "App and Environment/Scenes/Scene.code"
  "App and Environment/Scenes/SceneConfiguration.code"
  "App and Environment/Scenes/SceneDelegate.code"
  "App and Environment/Scenes/SceneSession.code"
  "App and Environment/Scenes/WindowScene.code"
  "App and Environment/Scenes/WindowSceneDelegate.code"
  "Appearance Customization/Configurations/BackgroundConfiguration.code"
  "Appearance Customization/Configurations/CellConfigurationState.code"
  "Appearance Customization/Configurations/ConfigurationColorTransformer.code"
  "Appearance Customization/Configurations/ConfigurationState.code"
  "Appearance Customization/Configurations/ConfigurationStateCustomKey.code"
  "Drag and Drop/SpringLoadedInteractionContext.code"
  "Drawing/BezierPath.code"
  "Drawing/Color.code"
  "Focus-Based Navigation/FocusAnimationCoordinator.code"
  "Focus-Based Navigation/FocusEnvironment.code"
  "Focus-Based Navigation/FocusItem.code"
  "Focus-Based Navigation/FocusItemContainer.code"
  "Focus-Based Navigation/FocusMovementHint.code"
  "Focus-Based Navigation/FocusUpdateContext.code"
  "Images and PDF/Image.code"
  "Keyboards and Input/TextInputTraits.code"
  "Menus and Shortcuts/Action.code"
  "Menus and Shortcuts/Command.code"
  "Menus and Shortcuts/ContextMenuConfiguration.code"
  "Menus and Shortcuts/ContextMenuInteractionAnimating.code"
  "Menus and Shortcuts/ContextMenuInteractionCommitAnimating.code"
  "Menus and Shortcuts/ContextMenuInteraction.code"
  "Menus and Shortcuts/ContextMenuInteractionDelegate.code"
  "Menus and Shortcuts/Menu.code"
  "Menus and Shortcuts/MenuBuilder.code"
  "Menus and Shortcuts/MenuElement.code"
  "Menus and Shortcuts/MenuSystem.code"
  "Menus and Shortcuts/PreviewParameters.code"
  "Menus and Shortcuts/PreviewTarget.code"
  "Menus and Shortcuts/TargetedPreview.code"
  "Pointer Interactions/PointerInteraction.code"
  "Pointer Interactions/PointerInteractionAnimating.code"
  "Pointer Interactions/PointerInteractionDelegate.code"
  "Pointer Interactions/PointerRegion.code"
  "Pointer Interactions/PointerRegionRequest.code"
  "Pointer Interactions/PointerStyle.code"
  "Text Display and Fonts/Font.code"
  "Text Display and Fonts/FontDescriptor.code"
  "Text Display and Fonts/FontMetrics.code"
  "Touches, Presses, and Gestures/Event.code"
  "Touches, Presses, and Gestures/GestureRecognizer.code"
  "Touches, Presses, and Gestures/GestureRecognizerDelegate.code"
  "Touches, Presses, and Gestures/Responder.code"
  "Touches, Presses, and Gestures/Touch.code"
  "Touches, Presses, and Gestures/Press.code"
  "Touches, Presses, and Gestures/PressesEvent.code"
  "View Controllers/ContentContainer.code"
  "View Controllers/PresentationController.code"
  "View Controllers/ViewController.code"
  "Views and Controls/Axis.code"
  "Views and Controls/BarItem.code"
  "Views and Controls/BarButtonItem.code"
  "Views and Controls/BarButtonItemGroup.code"
  "Views and Controls/BarPositioning.code"
  "Views and Controls/Button.code"
  "Views and Controls/Control.code"
  "Views and Controls/DatePicker.code"
  "Views and Controls/DirectionalEdgeInsets.code"
  "Views and Controls/DirectionalRectEdge.code"
  "Views and Controls/EdgeInsets.code"
  "Views and Controls/ImageView.code"
  "Views and Controls/Label.code"
  "Views and Controls/Offset.code"
  "Views and Controls/PickerView.code"
  "Views and Controls/ProgressView.code"
  "Views and Controls/Slider.code"
  "Views and Controls/Stepper.code"
  "Views and Controls/Switch.code"
  "Views and Controls/TextAlignment.code"
  "Views and Controls/TextField.code"
  "Views and Controls/TextView.code"
  "Views and Controls/View.code"
  "Views and Controls/VisualEffect.code"
  "Views and Controls/Table Views/ContextualAction.code"
  "Views and Controls/Table Views/SwipeActionsConfiguration.code"
  "Views and Controls/Table Views/TableView.code"
  "Views and Controls/Table Views/TableViewCell.code"
  "Views and Controls/Table Views/TableViewDataSource.code"
  "Views and Controls/Table Views/TableViewDelegate.code"
  "Views and Controls/Table Views/TableViewFocusUpdateContext.code"
  "Windows and Screens/AlertAction.code"
  "Windows and Screens/AlertController.code"
  "Windows and Screens/CoordinateSpace.code"
  "Windows and Screens/Screen.code"
  "Windows and Screens/Window.code")
target_sources(CodiraWin32 PRIVATE
  Platform/BatteryMonitor.code
  Platform/Error.code
  Platform/Win32+Menu.code
  Platform/Win32+PropertyWrappers.code
  Platform/WindowClass.code
  Platform/WindowsHandle.code)
target_sources(CodiraWin32 PRIVATE
  Support/Array+Extensions.code
  Support/Date+Extensions.code
  Support/IndexPath+UIExtensions.code
  Support/Logging.code
  Support/Rect+UIExtensions.code
  Support/Point+UIExtensions.code
  Support/Size+UIExtensions.code
  Support/String+UIExtensions.code
  Support/WindowsHandle+UIExtensions.code
  Support/WinSDK+Extensions.code
  Support/WinSDK+UIExtensions.code)
target_sources(CodiraWin32 PRIVATE
  Support/WindowMessage.code)
if(CMAKE_Codira_COMPILER_VERSION VERSION_LESS 5.7.0)
  target_sources(CodiraWin32 PRIVATE
    CoreAnimation/Transform3D.code)
  target_sources(CodiraWin32 PRIVATE
    CoreGraphics/AffineTransform.code
    CoreGraphics/Point.code
    CoreGraphics/Rect.code
    CoreGraphics/Size.code
    CoreGraphics/Vector.code)
else()
  add_library(CoreAnimation STATIC
    CoreAnimation/Transform3D.code)
  target_link_libraries(CodiraWin32 PRIVATE
    CoreAnimation)

  add_library(CoreGraphics STATIC
    CoreGraphics/AffineTransform.code
    CoreGraphics/Point.code
    CoreGraphics/Rect.code
    CoreGraphics/Size.code
    CoreGraphics/Vector.code)
  target_link_libraries(CodiraWin32 PRIVATE
    CoreGraphics)
endif()
target_link_libraries(CodiraWin32 PUBLIC
  ComCtl32
  User32)
target_link_libraries(CodiraWin32 PUBLIC
  CodiraCOM)
target_link_libraries(CodiraWin32 PUBLIC
  $<$<NOT:$<PLATFORM_ID:Darwin>>:dispatch>
  $<$<NOT:$<PLATFORM_ID:Darwin>>:Foundation>)
target_link_libraries(CodiraWin32 PRIVATE
  Logging
  OrderedCollections)
set_target_properties(CodiraWin32 PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Codira_MODULE_DIRECTORY}
  INTERFACE_LINK_DIRECTORIES $<TARGET_LINKER_FILE_DIR:CodiraWin32>)


if(CMAKE_Codira_COMPILER_VERSION VERSION_GREATER_EQUAL 5.7.0)
  _install_target(CoreAnimation)
  _install_target(CoreGraphics)
endif()
_install_target(CodiraWin32)

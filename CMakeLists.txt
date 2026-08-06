cmake_minimum_required(VERSION 3.21)

project(QThemeEngine
  VERSION 0.1.0
  DESCRIPTION "Qt Widgets theme engine: ThemeStore + QThemeStyle (QSS-free)"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

option(QTE_BUILD_SHARED "Build qte_engine as a shared library" ON)
option(QTE_BUILD_TESTS "Build unit tests" ON)
option(QTE_BUILD_EXAMPLES "Build example apps" ON)
option(QTE_BUILD_WIDGETS "Build owner-drawn sample widgets (separate target)" ON)
option(QTE_INSTALL "Install library, headers, and CMake package" ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 COMPONENTS PrintSupport OpenGLWidgets QUIET)
if(QTE_BUILD_TESTS)
  find_package(Qt6 REQUIRED COMPONENTS Test)
endif()

include(GNUInstallDirs)
include(GenerateExportHeader)

set(QTE_ENGINE_SOURCES
  src/qtheme/color_util.cpp
  src/qtheme/store.cpp
  src/qtheme/pack.cpp
  src/qtheme/accent.cpp
  src/qtheme/style/style_ctx.cpp
  src/qtheme/style/style_paint_util.cpp
  src/qtheme/style/style_edit.cpp
  src/qtheme/style/style_check.cpp
  src/qtheme/style/style_focus.cpp
  src/qtheme/style/style_button.cpp
  src/qtheme/style/style_menu.cpp
  src/qtheme/style/style_itemview.cpp
  src/qtheme/style/style_tab_header.cpp
  src/qtheme/style/style_progress.cpp
  src/qtheme/style/style_chrome.cpp
  src/qtheme/style/style_complex.cpp
  src/qtheme/style.cpp
  src/qtheme/engine.cpp
  src/qtheme/settings.cpp
  src/qtheme/api.cpp
  resources/theme.qrc
  include/qtheme/types.hpp
  include/qtheme/store.hpp
  include/qtheme/pack_registry.hpp
  include/qtheme/accent.hpp
  include/qtheme/theme_style.hpp
  include/qtheme/engine.hpp
  include/qtheme/settings.hpp
  include/qtheme/api.hpp
  include/qtheme/color_util.hpp
)

if(QTE_BUILD_SHARED)
  add_library(qte_engine SHARED ${QTE_ENGINE_SOURCES})
else()
  add_library(qte_engine STATIC ${QTE_ENGINE_SOURCES})
endif()
add_library(QThemeEngine::engine ALIAS qte_engine)

set(QTE_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
file(MAKE_DIRECTORY "${QTE_GENERATED_INCLUDE_DIR}/qtheme")
generate_export_header(qte_engine
  BASE_NAME QTE
  EXPORT_MACRO_NAME QTE_EXPORT
  EXPORT_FILE_NAME "${QTE_GENERATED_INCLUDE_DIR}/qtheme/qtheme_export.hpp"
)

target_include_directories(qte_engine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${QTE_GENERATED_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/qtheme>
)

target_link_libraries(qte_engine PUBLIC Qt6::Widgets)

set_target_properties(qte_engine PROPERTIES
  EXPORT_NAME engine
  OUTPUT_NAME qte_engine
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)

if(QTE_BUILD_WIDGETS)
  add_library(qte_demowidgets STATIC
    widgets/demobutton.cpp
    widgets/demobutton.hpp
  )
  target_include_directories(qte_demowidgets PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/widgets>
  )
  target_link_libraries(qte_demowidgets PUBLIC qte_engine Qt6::Widgets)
endif()

if(QTE_BUILD_EXAMPLES)
  add_executable(qte_demo
    examples/native_controls/main.cpp
    examples/native_controls/gallery_internal.cpp
    examples/native_controls/gallery_tabs_basic.cpp
    examples/native_controls/gallery_tabs_shell.cpp
    examples/native_controls/gallery_tabs_views.cpp
    examples/native_controls/gallery_tabs_misc.cpp
    examples/native_controls/gallery_coverage.cpp
    examples/native_controls/gallery.hpp
    examples/native_controls/gallery_internal.hpp
  )
  target_link_libraries(qte_demo PRIVATE qte_engine)
  if(TARGET qte_demowidgets)
    target_link_libraries(qte_demo PRIVATE qte_demowidgets)
  endif()
  if(TARGET Qt6::OpenGLWidgets)
    target_link_libraries(qte_demo PRIVATE Qt6::OpenGLWidgets)
    target_compile_definitions(qte_demo PRIVATE QTE_HAS_OPENGLWIDGETS=1)
  endif()
  if(TARGET Qt6::PrintSupport)
    target_link_libraries(qte_demo PRIVATE Qt6::PrintSupport)
    target_compile_definitions(qte_demo PRIVATE QTE_HAS_PRINTSUPPORT=1)
  endif()
  if(WIN32)
    set_target_properties(qte_demo PROPERTIES WIN32_EXECUTABLE TRUE)
  endif()

  if(NOT WINDEPLOYQT_EXECUTABLE)
    find_program(WINDEPLOYQT_EXECUTABLE
      NAMES windeployqt windeployqt.exe
      HINTS
        "$ENV{QTDIR}/bin"
        "${Qt6_DIR}/../../../bin"
      DOC "Qt windeployqt tool"
    )
  endif()
  if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET qte_demo POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_FILE:qte_engine>
              $<TARGET_FILE_DIR:qte_demo>
      COMMAND "${WINDEPLOYQT_EXECUTABLE}"
              --release
              --no-translations
              --compiler-runtime
              "$<TARGET_FILE:qte_demo>"
      COMMENT "Deploy qte_engine.dll + Qt runtime beside qte_demo"
      VERBATIM
    )
  elseif(WIN32)
    add_custom_command(TARGET qte_demo POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_FILE:qte_engine>
              $<TARGET_FILE_DIR:qte_demo>
      COMMENT "Copy qte_engine.dll beside qte_demo"
      VERBATIM
    )
    message(WARNING "windeployqt not found; qte_demo needs QTDIR/bin on PATH to run")
  endif()
endif()

if(QTE_BUILD_TESTS)
  enable_testing()
  add_executable(qte_tests tests/test_smoke.cpp)
  target_link_libraries(qte_tests PRIVATE qte_engine Qt6::Test)
  if(WIN32 AND QTE_BUILD_SHARED)
    add_custom_command(TARGET qte_tests POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_FILE:qte_engine>
              $<TARGET_FILE_DIR:qte_tests>
      COMMENT "Copy qte_engine.dll beside qte_tests"
      VERBATIM
    )
  endif()
  add_test(NAME qte_tests COMMAND qte_tests)
endif()

if(QTE_INSTALL)
  include(CMakePackageConfigHelpers)

  install(TARGETS qte_engine
    EXPORT QThemeEngineTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(DIRECTORY include/qtheme
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
  )
  install(FILES "${QTE_GENERATED_INCLUDE_DIR}/qtheme/qtheme_export.hpp"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qtheme
  )

  install(EXPORT QThemeEngineTargets
    FILE QThemeEngineTargets.cmake
    NAMESPACE QThemeEngine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QThemeEngine
  )

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QThemeEngineConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QThemeEngineConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QThemeEngine
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QThemeEngineConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QThemeEngineConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QThemeEngineConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QThemeEngine
  )
endif()

message(STATUS "QThemeEngine ${PROJECT_VERSION}")
message(STATUS "  QTE_BUILD_SHARED=${QTE_BUILD_SHARED} QTE_BUILD_EXAMPLES=${QTE_BUILD_EXAMPLES} QTE_BUILD_TESTS=${QTE_BUILD_TESTS} QTE_BUILD_WIDGETS=${QTE_BUILD_WIDGETS} QTE_INSTALL=${QTE_INSTALL}")

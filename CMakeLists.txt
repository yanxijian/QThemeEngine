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

option(QTE_BUILD_TESTS "Build unit tests" ON)
option(QTE_BUILD_EXAMPLES "Build example apps" ON)
option(QTE_BUILD_WIDGETS "Build owner-drawn sample widgets" ON)
option(QTE_INSTALL "Install library, headers, and CMake package" ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 COMPONENTS PrintSupport OpenGLWidgets QUIET)
if(QTE_BUILD_TESTS)
  find_package(Qt6 REQUIRED COMPONENTS Test)
endif()

include(GNUInstallDirs)

set(QTE_ENGINE_SOURCES
  src/qtheme/color_util.cpp
  src/qtheme/store.cpp
  src/qtheme/pack.cpp
  src/qtheme/accent.cpp
  src/qtheme/style.cpp
  src/qtheme/engine.cpp
  src/qtheme/settings.cpp
  src/qtheme/api.cpp
  # M1 Format stubs (XML loader)
  src/themeloader.cpp
  src/skinmanager.cpp
  src/themeapi.cpp
  src/color_util.cpp
  resources/theme.qrc
  include/qtheme/types.hpp
  include/qtheme/store.hpp
  include/qtheme/pack.hpp
  include/qtheme/accent.hpp
  include/qtheme/style.hpp
  include/qtheme/engine.hpp
  include/qtheme/settings.hpp
  include/qtheme/api.hpp
  include/qtheme/color_util.hpp
  include/theme/ithemeloader.hpp
  include/theme/iskinmanager.hpp
  include/theme/themeapi.hpp
  include/theme/themeloader.hpp
  include/theme/skinmanager.hpp
  include/theme/color_util.hpp
)

if(QTE_BUILD_WIDGETS)
  list(APPEND QTE_ENGINE_SOURCES
    widgets/demobutton.cpp
    widgets/demobutton.hpp
  )
endif()

add_library(qtheme_engine ${QTE_ENGINE_SOURCES})
add_library(QThemeEngine::engine ALIAS qtheme_engine)

target_include_directories(qtheme_engine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if(QTE_BUILD_WIDGETS)
  target_include_directories(qtheme_engine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/widgets>
  )
endif()

target_link_libraries(qtheme_engine PUBLIC Qt6::Widgets)

set_target_properties(qtheme_engine PROPERTIES
  EXPORT_NAME engine
  OUTPUT_NAME qtheme_engine
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(QTE_BUILD_EXAMPLES)
  add_executable(qtheme_demo
    examples/native_controls/main.cpp
    examples/native_controls/gallery_internal.cpp
    examples/native_controls/gallery_pages_basic.cpp
    examples/native_controls/gallery_pages_shell.cpp
    examples/native_controls/gallery_pages_views.cpp
    examples/native_controls/gallery_pages_misc.cpp
    examples/native_controls/gallery_session.cpp
    examples/native_controls/gallery.hpp
    examples/native_controls/gallery_internal.hpp
  )
  target_link_libraries(qtheme_demo PRIVATE qtheme_engine)
  if(TARGET Qt6::OpenGLWidgets)
    target_link_libraries(qtheme_demo PRIVATE Qt6::OpenGLWidgets)
    target_compile_definitions(qtheme_demo PRIVATE QTE_HAS_OPENGLWIDGETS=1)
  endif()
  if(TARGET Qt6::PrintSupport)
    target_link_libraries(qtheme_demo PRIVATE Qt6::PrintSupport)
    target_compile_definitions(qtheme_demo PRIVATE QTE_HAS_PRINTSUPPORT=1)
  endif()
  if(WIN32)
    set_target_properties(qtheme_demo PROPERTIES WIN32_EXECUTABLE TRUE)
  endif()

  # Deploy Qt (+ CRT) beside the demo so double-click works without PATH.
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
    add_custom_command(TARGET qtheme_demo POST_BUILD
      COMMAND "${WINDEPLOYQT_EXECUTABLE}"
              --release
              --no-translations
              --compiler-runtime
              "$<TARGET_FILE:qtheme_demo>"
      COMMENT "windeployqt: deploy Qt runtime beside qtheme_demo"
      VERBATIM
    )
  elseif(WIN32)
    message(WARNING "windeployqt not found; qtheme_demo needs QTDIR/bin on PATH to run")
  endif()
endif()

if(QTE_BUILD_TESTS)
  enable_testing()
  add_executable(qtheme_tests tests/test_smoke.cpp)
  target_link_libraries(qtheme_tests PRIVATE qtheme_engine Qt6::Test)
  add_test(NAME qtheme_tests COMMAND qtheme_tests)
endif()

if(QTE_INSTALL)
  include(CMakePackageConfigHelpers)

  install(TARGETS qtheme_engine
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
message(STATUS "  QTE_BUILD_EXAMPLES=${QTE_BUILD_EXAMPLES} QTE_BUILD_TESTS=${QTE_BUILD_TESTS} QTE_BUILD_WIDGETS=${QTE_BUILD_WIDGETS} QTE_INSTALL=${QTE_INSTALL}")

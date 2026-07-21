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

find_package(Qt6 REQUIRED COMPONENTS Widgets)
if(QTE_BUILD_TESTS)
  find_package(Qt6 REQUIRED COMPONENTS Test)
endif()

set(QTE_ENGINE_SOURCES
  src/qtheme/color_util.cpp
  src/qtheme/store.cpp
  src/qtheme/style.cpp
  src/qtheme/engine.cpp
  src/qtheme/api.cpp
  # M1 Format stubs (XML loader)
  src/themeloader.cpp
  src/skinmanager.cpp
  src/themeapi.cpp
  src/color_util.cpp
  resources/theme.qrc
  include/qtheme/types.hpp
  include/qtheme/store.hpp
  include/qtheme/style.hpp
  include/qtheme/engine.hpp
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
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
if(QTE_BUILD_WIDGETS)
  target_include_directories(qtheme_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/widgets")
endif()

target_link_libraries(qtheme_engine PUBLIC Qt6::Widgets)

if(QTE_BUILD_EXAMPLES)
  add_executable(qtheme_demo examples/native_controls/main.cpp)
  target_link_libraries(qtheme_demo PRIVATE qtheme_engine)
endif()

if(QTE_BUILD_TESTS)
  enable_testing()
  add_executable(qtheme_tests tests/test_smoke.cpp)
  target_link_libraries(qtheme_tests PRIVATE qtheme_engine Qt6::Test)
  add_test(NAME qtheme_tests COMMAND qtheme_tests)
endif()

message(STATUS "QThemeEngine ${PROJECT_VERSION}")
message(STATUS "  QTE_BUILD_EXAMPLES=${QTE_BUILD_EXAMPLES} QTE_BUILD_TESTS=${QTE_BUILD_TESTS} QTE_BUILD_WIDGETS=${QTE_BUILD_WIDGETS}")

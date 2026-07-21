cmake_minimum_required(VERSION 3.21)

project(QThemeEngine
  VERSION 0.1.0
  DESCRIPTION "Qt theme XML engine: load styles, switch skins, paint via ThemeApi"
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
option(QTE_BUILD_DEMO "Build L0 demo app" ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
if(QTE_BUILD_TESTS)
  find_package(Qt6 REQUIRED COMPONENTS Test)
endif()

add_library(qtheme_engine
  src/color_util.cpp
  src/themeloader.cpp
  src/skinmanager.cpp
  src/themeapi.cpp
  widgets/demobutton.cpp
  widgets/demobutton.hpp
  resources/theme.qrc
  include/theme/ithemeloader.hpp
  include/theme/iskinmanager.hpp
  include/theme/themeapi.hpp
  include/theme/themeloader.hpp
  include/theme/skinmanager.hpp
  include/theme/color_util.hpp
)
add_library(QThemeEngine::engine ALIAS qtheme_engine)

target_include_directories(qtheme_engine
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/widgets"
)
target_link_libraries(qtheme_engine PUBLIC Qt6::Widgets)

if(QTE_BUILD_DEMO)
  add_executable(qtheme_demo app/main.cpp)
  target_link_libraries(qtheme_demo PRIVATE qtheme_engine)
endif()

if(QTE_BUILD_TESTS)
  enable_testing()
  add_executable(qtheme_tests tests/test_smoke.cpp)
  target_link_libraries(qtheme_tests PRIVATE qtheme_engine Qt6::Test)
  add_test(NAME qtheme_tests COMMAND qtheme_tests)
endif()

message(STATUS "QThemeEngine ${PROJECT_VERSION}")
message(STATUS "  QTE_BUILD_DEMO=${QTE_BUILD_DEMO} QTE_BUILD_TESTS=${QTE_BUILD_TESTS}")

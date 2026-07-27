# QThemeEngine

[![CI](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](../../LICENSE)

Industrial Qt Widgets theming runtime: **ThemeStore + custom `QThemeStyle`**, replacing **QSS** (not generating StyleSheets).

> Canonical docs are Chinese — start at the [root README](../../README.md).

## Features

- **No QSS theme path**: `Engine::apply` clears stylesheets; painting goes through `QThemeStyle` (`QProxyStyle` + Fusion)
- **ThemeStore**: group/role → colors and metrics; Fluent packs (light / dark / hc) are SSOT
- **System integration**: Accent / ColorScheme, QSettings preferences, pack search paths
- **Product packaging**: `find_package(QThemeEngine)` → `QThemeEngine::engine`
- **Gallery demo**: Fluent control gallery under `examples/native_controls`

## Requirements

| Item | Notes |
|------|--------|
| Qt | **6.8+** (Widgets; Test for unit tests) |
| Toolchain | CMake 3.21+, Ninja; MSVC x64 (`vcvars`) on Windows |
| Optional | `clang-format` 20 for local format checks |

## Minimal usage

```cpp
QApplication app(argc, argv);
QCoreApplication::setOrganizationName(QStringLiteral("MyOrg"));
QCoreApplication::setApplicationName(QStringLiteral("MyApp"));

qtheme::Engine engine;
engine.apply(&app);
engine.loadPreferences();
engine.setAutoSavePreferences(true);
engine.switchSkin(QStringLiteral("dark"));
```

Do not mix `setStyleSheet` on UI themed by this engine. Color literals are `#RRGGBB` / `#RRGGBBAA` (not Qt `#AARRGGBB`).

## Build

```bat
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=%QTDIR%
cmake --build build
ctest --test-dir build --output-on-failure
```

CMake options: `QTE_BUILD_EXAMPLES`, `QTE_BUILD_TESTS`, `QTE_BUILD_WIDGETS`, `QTE_INSTALL`.  
Artifacts: library `qte_engine` (`QThemeEngine::engine`), demo `qte_demo`, tests `qte_tests`.  
CI: [ci.md](ci.md).

## Documentation

| Topic | 中文 | English |
|-------|------|---------|
| Architecture (canonical) | [../zh/architecture.md](../zh/architecture.md) | [architecture.md](architecture.md) |
| Dev plan | [../zh/dev-plan.md](../zh/dev-plan.md) | [dev-plan.md](dev-plan.md) |
| Coverage matrix | [../zh/coverage-matrix.md](../zh/coverage-matrix.md) | — |
| Widgets inventory | [../zh/qt-widgets-inventory.md](../zh/qt-widgets-inventory.md) | [qt-widgets-inventory.md](qt-widgets-inventory.md) |
| Format / tokens | [../zh/theme-engine-spec.md](../zh/theme-engine-spec.md) | — |
| CI | [../zh/ci.md](../zh/ci.md) | [ci.md](ci.md) |

## Milestones

| ID | Deliverable | Status |
|----|-------------|--------|
| M0–M0.5 | Store, Engine, Fluent packs, Accent | Done |
| M1 | Optional XML Format stub (JSON packs SSOT) | Stub only |
| M2–M6 | Fluent drawing coverage + install/Config | Done |

## License

Released under the [MIT License](../../LICENSE).

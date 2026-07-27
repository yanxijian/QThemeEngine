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

Local shared-library convention uses **`build-shared`** (CI may still use `-B build`).

```bat
:: vcvars x64 first; default SHARED, install to a local prefix
:: QTDIR = Qt 6.8+ prefix; PREFIX = install root (often sibling prefix/ of the three repos)
set PREFIX=<install-prefix>
cmake -S . -B build-shared -G Ninja ^
  -DCMAKE_PREFIX_PATH=%QTDIR% ^
  -DCMAKE_INSTALL_PREFIX=%PREFIX% ^
  -DQTE_BUILD_SHARED=ON -DQTE_INSTALL=ON
cmake --build build-shared
cmake --install build-shared
ctest --test-dir build-shared --output-on-failure
build-shared\qte_demo.exe
```

| CMake option | Role |
|--------------|------|
| `QTE_BUILD_SHARED` | Shared library (default ON); output name `qte_engine` |
| `QTE_BUILD_EXAMPLES` | Demo (`qte_demo`) |
| `QTE_BUILD_TESTS` | Unit tests (`qte_tests`) |
| `QTE_BUILD_WIDGETS` | Sample widgets target `qte_demowidgets` |
| `QTE_INSTALL` | Install + Config package (`QThemeEngine::engine`) |

Artifacts: `qte_engine` DLL / import lib, `qte_demo`, `qte_tests`.  
CI: [ci.md](ci.md).

## Documentation

| Topic | 中文 | English |
|-------|------|---------|
| Architecture (canonical) | [../zh/architecture.md](../zh/architecture.md) | [architecture.md](architecture.md) |
| Dev plan | [../zh/dev-plan.md](../zh/dev-plan.md) | [dev-plan.md](dev-plan.md) |
| Coverage matrix | [../zh/coverage-matrix.md](../zh/coverage-matrix.md) | — |
| Widgets inventory | [../zh/qt-widgets-inventory.md](../zh/qt-widgets-inventory.md) | [qt-widgets-inventory.md](qt-widgets-inventory.md) |
| Format (archive) | [../zh/theme-engine-spec.md](../zh/theme-engine-spec.md) | — |
| CI | [../zh/ci.md](../zh/ci.md) | [ci.md](ci.md) |

## Milestones

| ID | Deliverable | Status |
|----|-------------|--------|
| M0–M0.5 | Store, Engine, Fluent packs, Accent | Done |
| M2–M6 | Fluent drawing coverage + install/Config | Done |

> Former **M1** (`.theme.xml` Format) was cancelled; JSON packs only.

## License

Released under the [MIT License](../../LICENSE).

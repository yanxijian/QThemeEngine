# QThemeEngine

**English** · [简体中文](README.md)

[![CI](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Industrial Qt Widgets theming runtime: **ThemeStore + custom `QThemeStyle`**, replacing **QSS** (not generating StyleSheets).

## Features

- **No QSS theme path**: `Engine::apply` clears stylesheets; painting goes through `QThemeStyle` (`QProxyStyle` + Fusion)
- **ThemeStore**: group/role → colors and metrics; Fluent packs (light / dark / hc) are SSOT (`*.theme.json`)
- **System integration**: Accent / ColorScheme, QSettings preferences, pack search paths
- **Product packaging**: `find_package(QThemeEngine)` → `QThemeEngine::engine`
- **Gallery demo**: Fluent control gallery under `examples/native_controls`

## Requirements

| Item | Notes |
|------|--------|
| Qt | **6.8+** (Widgets; Test for unit tests) |
| Toolchain | CMake 3.21+, Ninja; MSVC x64 (`vcvars`) on Windows |
| Optional | `clang-format` 20 for local format checks |

Link Windows system libraries with `target_link_libraries` in `CMakeLists.txt`, not `#pragma comment(lib, …)`. See `.cursor/rules/cmake-windows-libs.mdc`.

## Quick start (Windows)

Minimal usage:

```cpp
QApplication app(argc, argv);
QCoreApplication::setOrganizationName(QStringLiteral("MyOrg"));
QCoreApplication::setApplicationName(QStringLiteral("MyApp"));

qtheme::Engine engine;
engine.apply(&app);
engine.loadPreferences();
engine.setAutoSavePreferences(true);
engine.switchPack(QString::fromUtf8(qtheme::kPackFluentDark));
```

Do not mix `setStyleSheet` on UI themed by this engine. Color literals are `#RRGGBB` / `#RRGGBBAA` (not Qt `#AARRGGBB`).

Local shared-library convention uses **`build-shared`** (CI may still use `-B build`).

```bat
:: vcvars x64 first; default SHARED, install to a local prefix
set QTDIR=<Qt-6.8+-prefix>
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

See [ci.md](docs/en/ci.md). Local format: `python scripts/format_source.py --check`.

## Layout

```text
include/qtheme/           Public API (Store / Style / Engine / Pack / Settings)
src/qtheme/               Implementation (style/* by control family)
resources/themes/fluent/  Fluent Theme Pack JSON (SSOT)
examples/native_controls/ Fluent gallery
widgets/                  Owner-drawn DemoButton (secondary)
cmake/                    QThemeEngineConfig.cmake.in
docs/zh|en/               Chinese + English docs
```

## Documentation

| Topic | 中文（主） | English |
|-------|------------|---------|
| Architecture (canonical) | [docs/zh/architecture.md](docs/zh/architecture.md) | [architecture.md](docs/en/architecture.md) |
| Dev plan | [docs/zh/dev-plan.md](docs/zh/dev-plan.md) | [dev-plan.md](docs/en/dev-plan.md) |
| Coverage matrix | [docs/zh/coverage-matrix.md](docs/zh/coverage-matrix.md) | — |
| Widgets inventory | [docs/zh/qt-widgets-inventory.md](docs/zh/qt-widgets-inventory.md) | [qt-widgets-inventory.md](docs/en/qt-widgets-inventory.md) |
| Format (archive) | [docs/zh/theme-engine-spec.md](docs/zh/theme-engine-spec.md) | — |
| CI | [docs/zh/ci.md](docs/zh/ci.md) | [ci.md](docs/en/ci.md) |

**Policy:** Prefer Chinese docs day-to-day. Product boundaries follow [architecture.md](docs/zh/architecture.md).

## Status

| Capability | Status |
|------------|--------|
| Store seed + `Engine::apply` + QThemeStyle (M0) | Done |
| Fluent packs + Accent / ColorScheme + pack merge (M0.5) | Done |
| Common Fluent control painting (M2–M4) | Done |
| Prefs, pack search paths, `find_package` install (M5) | Done |
| Deeper coverage (TextEdit / Frame / Splitter / Dock / Dial, M6) | Done |
| Near-term polish | See [dev-plan](docs/en/dev-plan.md) |

> Former **M1** (`.theme.xml` Format) was cancelled; JSON packs only.

## License

Released under the [MIT License](LICENSE).

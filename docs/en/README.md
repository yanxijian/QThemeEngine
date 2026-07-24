# QThemeEngine

Industrial Qt Widgets theming: **ThemeStore + custom `QThemeStyle`**, replacing **QSS** (not generating StyleSheets).

Canonical architecture (Chinese): [../zh/architecture.md](../zh/architecture.md)  
English summary: [architecture.md](architecture.md)  
Coverage matrix: [../zh/coverage-matrix.md](../zh/coverage-matrix.md)  
Widgets inventory: [../zh/qt-widgets-inventory.md](../zh/qt-widgets-inventory.md) · [qt-widgets-inventory.md](qt-widgets-inventory.md)

## Status

| Milestone | Focus | Status |
|-----------|--------|--------|
| **M0** | Store seeds + `Engine::apply` + QThemeStyle; native `QPushButton` recolors without QSS | **Done** |
| **M0.5** | Fluent packs (light/dark/hc) + Accent/ColorScheme + pack merge + T0 drawing | **Done** |
| M1 | Load `.theme.xml` (optional; JSON packs are SSOT) | Scaffold |
| **M2** | Button/Edit/Check/Combo/Spin/Menu/Tab/Header/ToolBar + state gallery | **Done** |
| **M3** | Slider/Progress/GroupBox/ToolTip + dpiScale | **Done** |
| M4+ | ItemViews, pack persistence | Planned |

## Build

```bat
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=%QTDIR%
cmake --build build
ctest --test-dir build --output-on-failure
```

Requires Qt 6 (Widgets; Test for unit tests) and an MSVC x64 / Ninja environment on Windows.

## License

MIT

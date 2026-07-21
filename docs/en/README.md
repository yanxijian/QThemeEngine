# QThemeEngine

Industrial Qt Widgets theming: **ThemeStore + custom `QThemeStyle`**, replacing **QSS** (not generating StyleSheets).

Canonical architecture (Chinese): [../zh/architecture.md](../zh/architecture.md)  
English summary: [architecture.md](architecture.md)  
Coverage matrix: [../zh/coverage-matrix.md](../zh/coverage-matrix.md)

## Status

| Milestone | Focus | Status |
|-----------|--------|--------|
| **M0** | Store seeds + `Engine::apply` + QThemeStyle; native `QPushButton` recolors without QSS | **Done** |
| M1 | Load `.theme.xml` | Scaffold (`ThemeLoader::setupXml` TODO) |
| M2+ | Expand coverage matrix | Not started |

## Build

```bat
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=%QTDIR%
cmake --build build
ctest --test-dir build --output-on-failure
```

Requires Qt 6 (Widgets; Test for unit tests) and an MSVC x64 / Ninja environment on Windows.

## License

MIT

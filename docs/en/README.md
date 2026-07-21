# QThemeEngine

Qt Widgets theme engine: **theme XML lookup tables + owner-drawn skins** (not StyleSheet).

Primary specification (Chinese): [../zh/theme-engine-spec.md](../zh/theme-engine-spec.md).

## Status

| Layer | Scope | Status |
|-------|--------|--------|
| L0 | Format + Loader + ThemeApi + DemoButton + Golden tests | Scaffold in-tree; `ThemeLoader::setupXml` / `SkinManager::switchSkin` still TODO (spec T2–T5) |
| L1 | Skin packages, extensions, persistence, DPI | Not started |
| L2 | Icon tinting, fixed surfaces, multi-process sync | Not started |

## Build

```bat
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=%QTDIR%
cmake --build build
ctest --test-dir build --output-on-failure
```

Requires Qt 6 (Widgets; Test for unit tests) and an MSVC x64 / Ninja environment on Windows.

## License

MIT

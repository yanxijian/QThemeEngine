# QThemeEngine (English)

Qt Widgets theme engine: **theme XML lookup table + owner-drawn skins** (not StyleSheet).

Sibling to [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell). Spec (Chinese primary): [../zh/style.md](../zh/style.md).

## Status

L0 scaffold is in-tree; `ThemeLoader::setupXml` / `SkinManager::switchSkin` still TODO per style.md T2–T5.

## Build

```bat
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=%QTDIR%
cmake --build build
ctest --test-dir build --output-on-failure
```

## License

MIT

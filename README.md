# QThemeEngine

Qt Widgets 主题引擎：**主题 XML 查表 + 控件自绘换肤**（不是 StyleSheet）。

与 [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell) 平级、独立复用。规格见 [docs/zh/theme-engine-spec.md](docs/zh/theme-engine-spec.md)。

## 状态

| 层 | 内容 | 状态 |
|----|------|------|
| L0 | Format + Loader + ThemeApi + DemoButton + Golden 测试 | **脚手架已建**；`ThemeLoader::setupXml` / `SkinManager::switchSkin` 待按 T2 实现 |
| L1 | 皮肤包、扩展包、持久化、DPI | 未开始 |
| L2 | 套色、固定表面、跨进程 | 未开始 |

## 目录

```text
include/theme/   公共头（IThemeLoader / ISkinManager / ThemeApi …）
src/             实现
widgets/         DemoButton
resources/       app.theme.xml（UTF-8 BOM）+ qrc
app/             L0 演示入口
tests/           烟测（完整 TC01–TC11 随 Loader 落地）
docs/zh/theme-engine-spec.md  机制 + 实现规格 + L0 可生成规格
```

## 构建（Windows）

```bat
:: vcvars x64，并设置 QTDIR（Qt 6.8+）
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=%QTDIR%
cmake --build build
ctest --test-dir build --output-on-failure
build\qtheme_demo.exe
```

## 约定

- 禁止对本主题化控件使用 `setStyleSheet`
- 颜色字面量：`#RRGGBB` / `#RRGGBBAA`（非 Qt `#AARRGGBB`）
- Token：加载期展开；换肤后 `reexpandAllTokenColors`
- 根目录 `.clang-format` 与 MultiProcessShell 对齐（可按需再调）

## License

MIT

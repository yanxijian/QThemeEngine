# QThemeEngine

Qt Widgets 工业化主题运行时：**ThemeStore + 自定义 `QThemeStyle`**，用主题表驱动原生控件绘制与度量，**替代 QSS**（不是生成 StyleSheet）。

权威架构：[docs/zh/architecture.md](docs/zh/architecture.md) · 覆盖矩阵：[docs/zh/coverage-matrix.md](docs/zh/coverage-matrix.md)  
格式 / Token / 历史 L0：[docs/zh/theme-engine-spec.md](docs/zh/theme-engine-spec.md)  
English：[docs/en/architecture.md](docs/en/architecture.md)

## 产品要点

1. **禁用 QSS** 作为主题通道（`Engine::apply` 默认清空 stylesheet）。  
2. **ThemeStore** 持有 group/role → 颜色与度量。  
3. **QThemeStyle**（`QProxyStyle` + Fusion 底座）查表绘制 Qt 自带控件。  
4. 自绘控件经 `qtheme::api` 读同一 Store（支线）。

## 里程碑状态

| ID | 交付 | 状态 |
|----|------|------|
| **M0** | Store seed + `Engine::apply` + QThemeStyle 骨架；原生 `QPushButton` 无 QSS 换色 | **已落地** |
| M1 | `.theme.xml` Format 加载 + light/dark Golden | 脚手架（`ThemeLoader::setupXml` 仍 TODO） |
| M2+ | 按覆盖矩阵扩展控件族 | 未开始 |

## 目录

```text
include/qtheme/              公共 API（Store / Style / Engine / api）
src/qtheme/                  实现
include/theme/ + src/        M1 Format 脚手架（过渡）
widgets/                     自绘 DemoButton（支线示例）
examples/native_controls/    原生控件 + 换肤演示
resources/                   Golden theme XML（M1）
docs/zh|en/
```

## 最小用法

```cpp
QApplication app(argc, argv);
qtheme::Engine engine;
engine.apply(&app);                 // 安装 QThemeStyle，seed light
engine.switchSkin(QStringLiteral("dark"));
```

## 构建（Windows）

```bat
:: vcvars x64，并设置 QTDIR（Qt 6.8+）
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=%QTDIR%
cmake --build build
ctest --test-dir build --output-on-failure
build\qtheme_demo.exe
```

CMake 选项：`QTE_BUILD_EXAMPLES`、`QTE_BUILD_TESTS`、`QTE_BUILD_WIDGETS`。

## 约定

- 禁止对本引擎主题化的 UI 混用 `setStyleSheet`
- 颜色字面量：`#RRGGBB` / `#RRGGBBAA`（非 Qt `#AARRGGBB`）
- 代码风格见根目录 `.clang-format`

## License

MIT

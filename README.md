# QThemeEngine

Qt Widgets 工业化主题运行时：**ThemeStore + 自定义 `QThemeStyle`**，用主题表驱动原生控件绘制与度量，**替代 QSS**（不是生成 StyleSheet）。

权威架构：[docs/zh/architecture.md](docs/zh/architecture.md) · 覆盖矩阵：[docs/zh/coverage-matrix.md](docs/zh/coverage-matrix.md)  
控件清单：[docs/zh/qt-widgets-inventory.md](docs/zh/qt-widgets-inventory.md) · 格式 / Token：[docs/zh/theme-engine-spec.md](docs/zh/theme-engine-spec.md)  
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
| **M0.5** | Fluent Pack（light/dark/hc）+ Accent/ColorScheme + Pack 注册/merge + T0 绘制扩展 | **已落地** |
| M1 | `.theme.xml` Format（可选兼容 stub；JSON Pack 为 SSOT） | stub only |
| **M2–M4** | 常用控件 Fluent 绘制（含 Spin/Menu/Slider/ItemView 等） | **已落地** |
| **M5** | 偏好持久化、Pack 搜索目录、`find_package(QThemeEngine)` 安装 | **已落地** |

## 目录

```text
include/qtheme/              公共 API（Store / Style / Engine / Pack / Settings）
src/qtheme/                  实现
include/theme/ + src/        可选 M1 XML 兼容 stub（非产品主路径）
widgets/                     自绘 DemoButton（支线示例）
examples/native_controls/    Fluent 画廊（按页拆分的 gallery_*.cpp）
resources/themes/fluent/     Fluent Theme Pack JSON（SSOT）
docs/zh|en/
cmake/                       QThemeEngineConfig.cmake.in
```

## 最小用法

```cpp
QApplication app(argc, argv);
QCoreApplication::setOrganizationName("MyOrg");
QCoreApplication::setApplicationName("MyApp");
qtheme::Engine engine;
engine.apply(&app);
engine.loadPreferences();
engine.setAutoSavePreferences(true);
engine.switchSkin(QStringLiteral("dark"));
```

## 构建（Windows）

```bat
:: vcvars x64，并设置 QTDIR（Qt 6.8+）
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=%QTDIR%
cmake --build build
ctest --test-dir build --output-on-failure
build\qtheme_demo.exe
cmake --install build --prefix dist
```

Demo 为 Fluent 控件画廊：Theme 菜单切换 `fluent.light` / `dark` / `hc` / System / `user.sample`，Accent 跟随系统或自定义；偏好经 QSettings 自动保存。

Windows 上编 `qtheme_demo` 会自动跑 `windeployqt`（含 CRT），把 Qt DLL 拷到 exe 旁，可直接双击运行。

CMake 选项：`QTE_BUILD_EXAMPLES`、`QTE_BUILD_TESTS`、`QTE_BUILD_WIDGETS`、`QTE_INSTALL`。
产品侧：`find_package(QThemeEngine)` → `QThemeEngine::engine`。

## 约定

- 禁止对本引擎主题化的 UI 混用 `setStyleSheet`
- 颜色字面量：`#RRGGBB` / `#RRGGBBAA`（非 Qt `#AARRGGBB`）
- 代码风格见根目录 `.clang-format`

## License

MIT

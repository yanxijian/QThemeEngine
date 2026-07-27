# QThemeEngine

[![CI](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/QThemeEngine/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Qt Widgets 工业化主题运行时：用 **ThemeStore + 自定义 `QThemeStyle`** 驱动原生控件绘制与度量，**替代 QSS**（不是生成 StyleSheet）。

> English overview：[docs/en/README.md](docs/en/README.md)

## 特性

- **无 QSS 主题通道**：`Engine::apply` 默认清空 stylesheet，绘制走 `QThemeStyle`（`QProxyStyle` + Fusion 底座）
- **ThemeStore**：`group` / `role` → 颜色与度量；Fluent Pack（light / dark / hc）为 SSOT（`*.theme.json`）
- **系统联动**：Accent / ColorScheme、偏好持久化（QSettings）、Pack 搜索目录
- **产品集成**：`find_package(QThemeEngine)` → `QThemeEngine::engine`
- **画廊 Demo**：`examples/native_controls` Fluent 控件画廊，可切换皮肤与 Accent

## 要求

| 项 | 说明 |
|----|------|
| Qt | **6.8+**（Widgets；单测另需 Test） |
| 工具链 | CMake 3.21+、Ninja；Windows 上 MSVC x64（`vcvars`） |
| 可选 | `clang-format` 20（本地格式检查） |

## 快速开始（Windows）

最小用法：

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

约定：对本引擎主题化的 UI **不要**再混用 `setStyleSheet`。颜色字面量为 `#RRGGBB` / `#RRGGBBAA`（非 Qt `#AARRGGBB`）。

本地共享库惯例使用构建目录 **`build-shared`**（与 `install_stack` / 旁路消费方一致）。CI 仍可能用 `-B build`。

```bat
:: 先 vcvars x64；默认 SHARED，安装到本地 prefix
:: QTDIR = Qt 6.8+ 前缀；PREFIX = 安装根（常与三仓同级的 prefix/）
set QTDIR=<Qt-6.8+-prefix>
set PREFIX=<install-prefix>
cmake -S . -B build-shared -G Ninja ^
  -DCMAKE_PREFIX_PATH=%QTDIR% ^
  -DCMAKE_INSTALL_PREFIX=%PREFIX% ^
  -DQTE_BUILD_SHARED=ON -DQTE_INSTALL=ON
cmake --build build-shared
cmake --install build-shared
build-shared\qte_demo.exe
```

Windows 上编 `qte_demo` 会拷贝 `qte_engine.dll` 并跑 `windeployqt`。

| CMake 选项 | 作用 |
|------------|------|
| `QTE_BUILD_SHARED` | 动态库（默认 ON）；产物名 `qte_engine` |
| `QTE_BUILD_EXAMPLES` | Demo（`qte_demo`） |
| `QTE_BUILD_TESTS` | 单测（`qte_tests`） |
| `QTE_BUILD_WIDGETS` | 自绘样例控件独立目标 `qte_demowidgets` |
| `QTE_INSTALL` | 安装与 Config 包（`QThemeEngine::engine`） |

CI 说明：[docs/zh/ci.md](docs/zh/ci.md) · 本地格式：`python scripts/format_source.py --check`

## 仓库布局

```text
include/qtheme/           公共 API（Store / Style / Engine / Pack / Settings）
src/qtheme/               实现（含 style/* 按控件族拆分）
resources/themes/fluent/  Fluent Theme Pack JSON（SSOT）
examples/native_controls/ Fluent 画廊
widgets/                  自绘 DemoButton（支线）
cmake/                    QThemeEngineConfig.cmake.in
docs/zh|en/               中英文文档
```

## 文档

| 主题 | 中文（主） | English |
|------|------------|---------|
| 架构（权威） | [architecture.md](docs/zh/architecture.md) | [architecture.md](docs/en/architecture.md) |
| 开发计划 | [dev-plan.md](docs/zh/dev-plan.md) | [dev-plan.md](docs/en/dev-plan.md) |
| 覆盖矩阵 | [coverage-matrix.md](docs/zh/coverage-matrix.md) | — |
| 控件清单 | [qt-widgets-inventory.md](docs/zh/qt-widgets-inventory.md) | [qt-widgets-inventory.md](docs/en/qt-widgets-inventory.md) |
| 格式归档 | [theme-engine-spec.md](docs/zh/theme-engine-spec.md) | — |
| CI | [ci.md](docs/zh/ci.md) | [ci.md](docs/en/ci.md) |

**约定**：日常以中文文档为准；英文为同步译本。产品边界以 [architecture.md](docs/zh/architecture.md) 为准。

## 现状

| 能力 | 状态 |
|------|------|
| Store seed + `Engine::apply` + QThemeStyle（M0） | 完成 |
| Fluent Pack + Accent / ColorScheme + Pack merge（M0.5） | 完成 |
| 常用控件 Fluent 绘制（Button / Edit / Menu / Slider / ItemView 等，M2–M4） | 完成 |
| 偏好持久化、Pack 搜索目录、`find_package` 安装（M5） | 完成 |
| 覆盖加深（TextEdit / Frame / Splitter / Dock / Dial 等，M6） | 完成 |
| 近中期打磨 / 加深 | 见 [dev-plan](docs/zh/dev-plan.md) |

> 曾规划的 **M1**（`.theme.xml`）已取消；主题数据仅支持 JSON Pack。

## License

Released under the [MIT License](LICENSE).

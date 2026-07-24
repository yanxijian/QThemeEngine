# QThemeEngine 架构说明（产品主路径）

> **English**：[../en/architecture.md](../en/architecture.md)  
> **地位**：工程与产品方向的**权威说明**。  
> **关系**：[`theme-engine-spec.md`](theme-engine-spec.md) 保留主题格式 / Token / 加载算法与历史 L0 细节；若与本文冲突，**以本文的产品目标与模块边界为准**。

---

## 1. 产品目标

QThemeEngine 是面向 **Qt Widgets** 的工业化主题运行时：

1. **禁用 QSS**（`QWidget::setStyleSheet` / `QApplication::setStyleSheet` 不作为主题通道）。  
2. 用 **主题表（Token → Control Role）** 描述颜色、字体、度量（边距、间距、圆角、默认高度等）。  
3. 用自定义 **`QThemeStyle`（QStyle）** 绘制并度量 **Qt 自带控件**，在能力上**替代 QSS**。  
4. 自绘控件可通过同一 **ThemeStore** 查表绘制（支线，非主证明路径）。

**一句话**：主题包装进 Store；Style 查表画原生控件；换肤换数据，不换 QSS 字符串。

---

## 2. 设计原则

| 原则 | 说明 |
|------|------|
| 数据与绘制分离 | XML/包只产 Store；绘制只在 QStyle / 自绘中查表 |
| 单一数据源 | Palette、Style、自绘 API 读同一 Store |
| 渐进覆盖 | 未实现的 CE_/PM_ 回退基类 Style；用覆盖矩阵管理 |
| 可测试 | Store Golden 可不启 GUI；Style 度量可单测 |
| 可嵌入 | `qtheme::Engine::apply(QApplication*)`；无强制业务单例逻辑进库 |
| 禁止双轨 | 同一 UI 树不得混用本引擎与 QSS |

---

## 3. 逻辑架构

```text
                    ┌─────────────────┐
                    │   Theme Pack    │  (.theme.xml / 资源)
                    └────────┬────────┘
                             │ Format / Loader
                             ▼
                    ┌─────────────────┐
                    │   ThemeStore    │  group + role → Color / Font / Metric
                    └────────┬────────┘
           ┌─────────────────┼─────────────────┐
           ▼                 ▼                 ▼
    ┌────────────┐   ┌────────────┐   ┌──────────────┐
    │ QThemeStyle│   │ ThemeApi   │   │ QPalette     │
    │ (主路径)   │   │ (自绘支线) │   │ from Store   │
    └─────┬──────┘   └────────────┘   └──────────────┘
          │
          ▼
   QApplication::setStyle
   原生 QPushButton / QLineEdit / …
```

### 3.1 模块职责

| 模块 | 职责 | 非职责 |
|------|------|--------|
| **Format** | 解析主题文件、import、relatedName | 不接触 QPainter |
| **ThemeStore** | 查询与换肤原子替换；Token 已展开 | 不实现 CE_* |
| **QThemeStyle** | `draw*` / `pixelMetric` / `sizeFromContents` / `subElementRect`… | 不读磁盘 XML |
| **Engine** | 加载肤、`apply`、信号、DPI 策略入口 | 不承载业务窗口 |
| **ThemeApi** | 薄门面，供自绘 | 不替代 QStyle |

### 3.2 推荐命名空间与头文件

- C++：`namespace qtheme`  
- Include：`#include <qtheme/engine.hpp>` 等  
- Qt 动态属性（自绘）：`qtheme.class`（常量 `qtheme::kClassProperty`）

---

## 4. 主题数据模型

### 4.1 两层

1. **Token（语义）**（Store：`color("palette", role)` / `metric("button", role)`）  
   例：`palette`/`window`、`palette`/`base`、`palette`/`text`、`palette`/`accent`、`palette`/`canvas`、`palette`/`stroke`。  
   度量目前落在控件族上（如 `button`/`radius`、`button`/`padding`、`button`/`height`）；尚未单独使用 `size.*` 命名空间。

2. **Control Role（控件 × 部件 × 状态）**  
   例：`button`/`bg`、`button`/`bg.hover`、`button`/`fg`、`button`/`border`、`button`/`border.focus`  
   `edit`/`bg`、`edit`/`border.focus`、`check`/`bg.checked`、`tab`/`indicator`、`scroll`/`handle`。

Style 内映射示例：

```text
CE_PushButtonBevel + State_MouseOver  →  color("button", "bg.hover")
PM_ButtonMargin                      →  metric("button", "padding")
```

状态后缀约定（与规格兼容）：`.hover` / `.pressed`（或 `-hover` 持久化名，实现层统一）/ `.disabled` / `.focus` 等；**默认单后缀**。

### 4.2 度量与 DPI

- Store 存 **逻辑像素**（设计稿 100%）。  
- `QThemeStyle` 统一乘 `dpiScale`（由 Engine 根据 `QWindow`/`QScreen` 或应用策略给出）。  
- 颜色不缩放。

### 4.3 亮 / 暗 / 高对比度（Fluent 主题族）

同一 Role 名；多套 **Theme Pack**（`fluent.light` / `fluent.dark` / `fluent.hc`）。`Engine::switchSkin` / `setColorScheme` 替换 Store 并刷新 Style。

历史别名：`light` → `fluent.light`，`dark` → `fluent.dark`。

### 4.4 Theme Pack 契约

Pack 是可注册的主题数据单元（内置 qrc 或用户文件），加载后展开为 `ThemeStore`。

**JSON 是内置 Fluent Pack 的唯一数据源**（`resources/themes/fluent/*.json` → qrc）。C++ 侧只做加载 / 注册 / merge，**禁止**再维护一套 `fluentLight()` / `fluentDark()` / `fluentHighContrast()` 全量 Token 双轨。

| 字段 | 说明 |
|------|------|
| `id` | 唯一 id，如 `fluent.light`、`user.brand` |
| `base` | 可选；先加载基包再 merge 覆盖（用户派生 Pack） |
| `colors` | `group → role → #RRGGBB(AA)` |
| `metrics` | `group → role → int`（逻辑像素） |

**稳定语义 Token（跨皮同名，与 JSON/`ThemeStore` 一致）**：`palette.window` / `base` / `text` / `accent` / `accent.text` / `stroke` / `highlight` / `canvas` / `surface` …  
**控件 Role**：`button.*` / `edit.*` / `check.*` / `tab.*` / `scroll.*` / `menu.*` / `toolbar.*` / `combo.*` / `spin.*` / `slider.*` / `progress.*` / `groupbox.*` / `tooltip.*` / `header.*`  
**度量 Role**：逻辑像素；`QThemeStyle` 统一 × `dpiScale`（Engine 按主屏 `logicalDotsPerInch/96` 更新）。

校验：缺必需 Token 时 `Engine` 报错并保持旧 Store；缺可选 Role 时 Style 回退基类。

### 4.5 Accent 与 ColorScheme（Engine 策略，非第二套 Style）

```text
setColorScheme(Light|Dark|HighContrast|System)
  → 选定 Pack id → 构建 Store → 若 Accent=System 则 AccentResolver 补丁 → 刷新

setAccent(QColor | System)
  → 不换 Pack id；补丁 palette.accent / highlight / *.border.focus 等衍生色 → 刷新
```

`AccentResolver`：优先读平台强调色（Qt palette Highlight / 风格提示），失败则用 Pack 默认 `palette.accent`。Windows 上另读 `SPI_GETHIGHCONTRAST`（`systemHighContrast()`）。

**OS 外观监听（`Engine::apply` 后安装）**：
- `QGuiApplication::paletteChanged` → 跟随系统 Accent；`ColorScheme::System` 时重解析 Pack（含 HC）。
- `QStyleHints::colorSchemeChanged` → `ColorScheme::System` 时在 light/dark/hc 间切换。
- `setFollowOsHighContrast(true)`（默认）：System 下 OS HC 开则选用 `fluent.hc`。

### 4.6 用户自定义（同管道）

| 档位 | 能力 |
|------|------|
| A. Token 覆写 | merge 局部 colors/metrics 到当前 Store |
| B. 派生 Pack | JSON `base` + overrides，`registerPack` |
| C. 完整 Pack | 与内置同级 `registerPack` + `switchSkin` |

禁止用 QSS 作为自定义通道。

**Engine API（Fluent 阶段）**：

```text
registerPack(path|qrc) → bool
switchSkin(id)
setAccent(QColor) / setAccentFollowSystem(bool)
setColorScheme(ColorScheme)
currentSkin() / colorScheme() / accent()
```

---

## 5. QThemeStyle 策略

1. **基类**：`QProxyStyle`，底座推荐 **Fusion**（跨平台一致、可预测）。  
2. **已覆盖**：查 Store 自绘或调整 metric。  
3. **未覆盖**：`QProxyStyle::*` 回退。  
4. **`standardPalette()`**：由 Store Token 填充，减少漏画控件的违和。  
5. **禁止**依赖 QSS；`apply` 时可清空 application/widget stylesheet（可配置，默认建议清空）。

扩展顺序见 [coverage-matrix.md](coverage-matrix.md)。

---

## 6. 换肤时序

```text
switchSkin(name)
  → 解析并构建新 ThemeStore（失败则保持旧 Store）
  → 原子替换
  → 更新 QThemeStyle 持有的 Store 指针 / 版本号
  → standardPalette + polish
  → emit skinChanged
  → 顶层窗口 update
```

缓存（刷子、图标着色）必须绑定 **Store 世代（generation）**，换肤后失效。

批量改 Token 时用 `ThemeStore::beginUpdate()` / `endUpdate()`，只 bump 一次 generation（Accent 补丁、JSON overlay 加载已按此批量）。

---

## 7. 工程目录（目标形态）

```text
include/qtheme/          公共 API
src/qtheme/              实现（store / style / engine / format / api）
examples/                原生控件演示（无 QSS）
tests/                   Store Golden + Style 度量
docs/zh|en/
resources/               内置 fallback 主题
cmake/                   后续：安装与 Config 包
```

CMake 选项建议：`QTE_BUILD_EXAMPLES`、`QTE_BUILD_TESTS`、`QTE_BUILD_WIDGETS`（自绘示例控件）。

---

## 8. 里程碑

| 里程碑 | 交付 | 验收 |
|--------|------|------|
| **M0** | Store API + 可编程 seed + Engine::apply + QThemeStyle 骨架 | **已交付**：无 QSS；原生 QPushButton 随 seed 换色 |
| **M0.5** | Fluent Pack 族 + Accent/HC 策略 + Pack 注册/merge + T0 绘制扩展 | **已交付**：单一 QThemeStyle + 多 Pack |
| **M1** | Format 加载 `.theme.xml` + light/dark Golden | 可选/兼容；JSON Pack 为 SSOT |
| **M2** | Button / Edit / Check / Combo / Spin / Menu / Tab / Header / ToolBar | **已交付**（含状态页验收） |
| **M3** | Slider / Progress / GroupBox / ToolTip + DPI | **已交付**：度量 × `dpiScale`（96DPI=1.0） |
| **M4** | ItemView 选中/hover/交替行 | 矩阵 Views 行 |
| **M5** | 皮肤包、扩展、持久化 | 产品集成 |

---

## 9. 非目标（本阶段）

- 主题商城 / 在线运营  
- Qt Quick Controls 主题（另案）  
- 用 QSS 生成器「兼容旧皮肤」作为主路径  
- 第一天 100% 像素复刻所有 CE_*  

---

## 10. 相关文档

| 文档 | 角色 |
|------|------|
| [architecture.md](architecture.md) | **本文：产品主路径** |
| [coverage-matrix.md](coverage-matrix.md) | 控件覆盖矩阵（实现进度） |
| [qt-widgets-inventory.md](qt-widgets-inventory.md) | Qt Widgets 控件清单与主题优先级 |
| [theme-engine-spec.md](theme-engine-spec.md) | 格式 / Token / 加载细节与历史 L0 |
| [../en/architecture.md](../en/architecture.md) | English architecture summary |

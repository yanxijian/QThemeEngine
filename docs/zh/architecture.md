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

1. **Token（语义）**  
   例：`palette.canvas`、`palette.text.primary`、`palette.accent`、`size.radius.md`、`size.space.sm`、`size.control.height`。

2. **Control Role（控件 × 部件 × 状态）**  
   例：`button.bg`、`button.bg.hover`、`button.fg`、`button.border`、`button.radius`、`button.padding`  
   `edit.bg`、`edit.border.focus`、`check.indicator`、`scroll.handle`。

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

### 4.3 亮 / 暗

同一 Role 名；两套 Token 包（如 `light` / `dark`）。`Engine::switchSkin` 替换 Store 并刷新 Style。

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
| **M1** | Format 加载 `.theme.xml` + light/dark Golden | TC 色表；换肤信号 |
| **M2** | Button / Edit / Check 族覆盖 | 覆盖矩阵对应行变绿 |
| **M3** | 度量全面主题化 + DPI | padding/radius/height 可配 |
| **M4** | ItemView / Menu / Tab 等 | 矩阵继续扩展 |
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
| [coverage-matrix.md](coverage-matrix.md) | 控件覆盖矩阵 |
| [theme-engine-spec.md](theme-engine-spec.md) | 格式 / Token / 加载细节与历史 L0 |
| [../en/architecture.md](../en/architecture.md) | English architecture summary |

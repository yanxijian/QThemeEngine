# 控件覆盖矩阵（QThemeStyle）

> 与 [architecture.md](architecture.md) 配套。  
> **控件全集盘点**（含邻接模块与主题优先级）：[qt-widgets-inventory.md](qt-widgets-inventory.md)  
> **图例**：`—` 未开始 · `◐` 进行中 / 部分 · `●` 已主题化（查表绘制或度量）· `○` 明确回退基类（可接受）

换肤验收以 **无 QSS + 本表已标 ● 的控件** 为准；未标 ● 的允许 Fusion 默认观感。

---

## v0.1 目标（M0–M2）

| 控件族 | 代表类 | 绘制 | 度量 (PM_/size) | Palette | 备注 |
|--------|--------|------|-----------------|---------|------|
| 应用级 | `QApplication` | — | — | ● seed | `standardPalette` |
| 按钮 | `QPushButton` | ◐ Fluent | ◐ | ● | M0.5 Fluent bevel |
| 工具按钮 | `QToolButton` | — | — | ● | M2 |
| 勾选 | `QCheckBox` | ◐ | ◐ | ● | PE_IndicatorCheckBox |
| 单选 | `QRadioButton` | ◐ | ◐ | ● | PE_IndicatorRadioButton |
| 单行编辑 | `QLineEdit` | ◐ | — | ● | PE_FrameLineEdit |
| 数字框 | `QSpinBox` | — | — | ● | M2+ |
| 下拉 | `QComboBox` | — | — | ● | M2+ |
| 标签 | `QLabel` | ○ | ○ | ● | 多靠 Palette |
| 滚动条 | `QScrollBar` | ◐ | ◐ | — | CC_ScrollBar |
| 滑块 | `QSlider` | — | — | — | M3 |
| 进度 | `QProgressBar` | — | — | — | M3 |
| 标签页 | `QTabBar`/`QTabWidget` | ◐ | ◐ | — | CE_TabBarTab* |
| 工具栏 | `QToolBar` | — | — | — | M4 |
| 菜单 | `QMenu`/`QMenuBar` | — | — | — | M4 |
| 视图 | `QTreeView`/`QTableView`/`QListView` | — | — | — | M4 |
| 表头 | `QHeaderView` | — | — | — | M4 |
| 分组 | `QGroupBox` | — | — | — | M3 |
| 提示 | `QToolTip` | — | — | — | M3 |

---

## Role 命名（控件族前缀）

| 前缀 | 用途 |
|------|------|
| `palette.*` | 全局语义 Token |
| `button.*` | 按钮面/文/边/圆角/边距 |
| `edit.*` | 输入框 |
| `check.*` | 复选 / 单选指示器 |
| `scroll.*` | 滚动条 |
| `slider.*` | 滑块 |
| `tab.*` | 标签页 |
| `menu.*` | 菜单 |
| `view.*` | ItemView |
| `header.*` | 表头 |

状态：`bg` / `bg.hover` / `bg.pressed` / `bg.disabled` / `bg.checked` / `border.focus` 等。

---

## 维护规则

1. 合并「接管某 CE_/PM_」的 PR 必须更新本表。  
2. 新增 Role 须在主题 seed / XML 与 Style 映射表三处一致。  
3. CI 可对 ● 行做冒烟（后续）：创建控件 → 无 stylesheet → 换肤 → 抽样色或 metric。

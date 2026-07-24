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
| 按钮 | `QPushButton` | ● Fluent | ● | ● | N/H/P/D + Default accent |
| 工具按钮 | `QToolButton` | ● Fluent | ◐ | ● | `CC_ToolButton` / `PE_PanelButtonTool` |
| 勾选 | `QCheckBox` | ● | ● | ● | N/H/Checked/Disabled |
| 单选 | `QRadioButton` | ● | ● | ● | 同 check |
| 单行编辑 | `QLineEdit` | ● | ◐ | ● | N/H/Focus/Disabled |
| 数字框 | `QSpinBox` | — | — | ● | M2+ |
| 下拉 | `QComboBox` | ● Fluent | ◐ | ● | N/H/P/D/Focus |
| 标签 | `QLabel` | ○ | ○ | ● | 多靠 Palette |
| 滚动条 | `QScrollBar` | ● | ● | — | handle + arrows；N/H/P/D |
| 滑块 | `QSlider` | — | — | — | M3 |
| 进度 | `QProgressBar` | — | — | — | M3 |
| 标签页 | `QTabBar`/`QTabWidget` | ● | ◐ | — | N/H/Selected/Disabled |
| 工具栏 | `QToolBar` | ● | ◐ | — | `CE_ToolBar` |
| 菜单 | `QMenu`/`QMenuBar` | ● | ◐ | — | item / bar / separator |
| 视图 | `QTreeView`/`QTableView`/`QListView` | — | — | — | M4 |
| 表头 | `QHeaderView` | ● | ◐ | — | section + label；N/H/P/D |
| 分组 | `QGroupBox` | — | — | — | M3 |
| 提示 | `QToolTip` | — | — | — | M3 |

---

## Role 命名（控件族前缀）

| 前缀 | 用途 |
|------|------|
| `palette.*` | 全局语义 Token |
| `button.*` | 按钮面/文/边/圆角/边距 / accent |
| `edit.*` | 输入框 |
| `check.*` | 复选 / 单选指示器 |
| `scroll.*` | 滚动条 |
| `slider.*` | 滑块 |
| `tab.*` | 标签页 |
| `menu.*` | 菜单 / 菜单栏 |
| `toolbar.*` | 工具栏 |
| `combo.*` | 下拉框 |
| `header.*` | 表头 |
| `view.*` | ItemView |

状态：`bg` / `bg.hover` / `bg.pressed` / `bg.disabled` / `bg.checked` / `border.focus` 等。

---

## 维护规则

1. 合并「接管某 CE_/PM_」的 PR 必须更新本表。  
2. 新增 Role 须在主题 JSON Pack 与 Style 映射表一致（禁止 C++ 全量双轨）。  
3. CI 可对 ● 行做冒烟（后续）：创建控件 → 无 stylesheet → 换肤 → 抽样色或 metric。

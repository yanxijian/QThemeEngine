# 控件覆盖矩阵（QThemeStyle）

> 与 [architecture.md](architecture.md) 配套。  
> **控件全集盘点**（含邻接模块与主题优先级）：[qt-widgets-inventory.md](qt-widgets-inventory.md)  
> **图例**：`—` 未开始 · `◐` 进行中 / 部分 · `●` 已主题化（查表绘制或度量）· `○` 明确回退基类（可接受）

换肤验收以 **无 QSS + 本表已标 ● 的控件** 为准；未标 ● 的允许 Fusion 默认观感。

---

## v0.1 + M6 目标

| 控件族 | 代表类 | 绘制 | 度量 (PM_/size) | Palette | 备注 |
|--------|--------|------|-----------------|---------|------|
| 应用级 | `QApplication` | — | — | ● seed | `standardPalette` |
| 按钮 | `QPushButton` | ● Fluent | ● | ● | N/H/P/D + Default accent |
| 命令链接 | `QCommandLinkButton` | ● | ○ | ● | `CE_PushButtonLabel` + `commandlink.*` |
| 工具按钮 | `QToolButton` | ● Fluent | ● | ● | `CC_ToolButton` / height |
| 勾选 | `QCheckBox` | ● | ● | ● | N/H/Checked/Disabled |
| 单选 | `QRadioButton` | ● | ● | ● | 同 check |
| 单行编辑 | `QLineEdit` | ● | ● | ● | N/H/Focus/Disabled；height/padding |
| 多行编辑 | `QPlainTextEdit`/`QTextEdit` | ● Fluent | ◐ | ● | `textedit.*` + `PE_Frame` |
| 数字框 | `QSpinBox`/`QDoubleSpinBox` | ● Fluent | ● | ● | N/H/Focus/Disabled + 上下按钮 |
| 日期时间 | `QDateEdit`/`QTimeEdit`/`QDateTimeEdit` | ● | ● | ● | 复用 `CC_SpinBox` / spin 帧 |
| 月历 | `QCalendarWidget` | ● | ○ | ● | `calendar.*` 面板 + view 格子 |
| 下拉 | `QComboBox` | ● Fluent | ● | ● | N/H/P/D/Focus |
| 标签 | `QLabel` | ○ | ○ | ● | 多靠 Palette |
| 滚动条 | `QScrollBar` | ● | ● | — | handle + arrows；N/H/P/D |
| 滑块 | `QSlider` | ● Fluent | ● | — | groove/fill/handle；N/H/P/D/Focus |
| 拨盘 | `QDial` | ● Fluent | ● | — | `CC_Dial` + `dial.*` |
| 进度 | `QProgressBar` | ● Fluent | ● | — | groove/chunk；`qtheme.progressState`=paused/error |
| 标签页 | `QTabBar`/`QTabWidget` | ● | ● | — | N/H/Selected/Disabled；tab.height |
| 工具栏 | `QToolBar` | ● | ● | — | `CE_ToolBar` + handle/separator/PM_ |
| 菜单 | `QMenu`/`QMenuBar` | ● | ● | — | itemHeight / bar / separator |
| 视图 | `QTreeView`/`QTableView`/`QListView` | ● Fluent | ● | ● | 选中/hover/交替行/branch |
| 表头 | `QHeaderView` | ● | ● | — | section + label；header.height |
| 分组 | `QGroupBox` | ● | ● | — | `PE_FrameGroupBox` + `CT_GroupBox` padding |
| 提示 | `QToolTip` | ● | ● | ● | `PE_PanelTipLabel` + `PM_ToolTipLabelFrameWidth` |
| 线框 | `QFrame` | ● | ◐ | — | StyledPanel / HLine / VLine |
| 滚动区 | `QScrollArea` | ● | ○ | — | 外框走 `textedit`/`PE_Frame` |
| 分割条 | `QSplitter` | ● | ● | — | `CE_Splitter` + `PM_SplitterWidth` |
| 停靠 | `QDockWidget` | ● | ● | — | title / frame / resize handle |
| 状态栏 | `QStatusBar` | ● | ◐ | — | `PE_PanelStatusBar` / item |
| 对话框按钮 | `QDialogButtonBox` | ○ | ○ | ● | 复用 Button；间距靠布局 |

---

## Role 命名（控件族前缀）

| 前缀 | 用途 |
|------|------|
| `palette.*` | 全局语义 Token（含 `text.tertiary` / `accent.*` / `focus.outer|inner`） |
| `button.*` | 按钮面/文/边/圆角/边距 / `bg.accent*` / `fg.accent*` |
| `commandlink.*` | 命令链接标题/说明 |
| `edit.*` | 输入框（含 `fg.placeholder` → `QPalette::PlaceholderText`） |
| `textedit.*` | 多行编辑 / 滚动区外框（含 `fg.placeholder`） |
| `check.*` | 复选 / 单选指示器（含 `bg.checked.hover`） |
| `scroll.*` | 滚动条 |
| `tab.*` | 标签页 |
| `menu.*` | 菜单 / 菜单栏（含 `bg.acrylic` 实色近似） |
| `toolbar.*` | 工具栏 |
| `combo.*` | 下拉框（含 `fg.placeholder`） |
| `spin.*` | 数字框 / 日期时间步进 |
| `slider.*` | 滑块（含 `fill.hover`） |
| `dial.*` | 拨盘 |
| `progress.*` | 进度条（含 `chunk.paused` / `chunk.error`） |
| `groupbox.*` | 分组框 |
| `tooltip.*` | 工具提示 |
| `header.*` | 表头 |
| `view.*` | ItemView（含 `bg.selected.hover`） |
| `frame.*` | QFrame 面板与分隔线 |
| `splitter.*` | 分割条 |
| `dock.*` | 停靠窗 |
| `status.*` | 状态栏 |
| `calendar.*` | 月历外壳 |

状态：`bg` / `bg.hover` / `bg.pressed` / `bg.disabled` / `bg.checked` / `bg.selected.hover` / `border.focus` 等。

---

## 维护规则

1. 合并「接管某 CE_/PM_」的 PR 必须更新本表。  
2. 新增 Role 须在主题 JSON Pack 与 Style 映射表一致（禁止 C++ 全量双轨）。  
3. CI 可对 ● 行做冒烟（后续）：创建控件 → 无 stylesheet → 换肤 → 抽样色或 metric。

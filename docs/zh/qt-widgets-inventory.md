# Qt Widgets 控件清单（主题覆盖参考）

> **English**：[../en/qt-widgets-inventory.md](../en/qt-widgets-inventory.md)  
> **地位**：Qt 6 Widgets（及邻接模块）控件与配套 API 的**盘点清单**，供 QThemeEngine 规划覆盖范围。  
> **权威实现进度**：[coverage-matrix.md](coverage-matrix.md)  
> **产品主路径**：[architecture.md](architecture.md)  
> **上游对照**：[Qt Widget Classes](https://doc.qt.io/qt-6/widget-classes.html)、本机 `Qt6::Widgets` 公开头文件

---

## 0. 与本仓库的关系

QThemeEngine 用 **ThemeStore + `QThemeStyle`（`QProxyStyle`）** 替代 **QSS** 主题通道。本清单回答「有哪些原生控件可能被画到」；**是否已主题化**以覆盖矩阵为准。

| 通道 | 说明 |
|------|------|
| **主路径** | `QThemeStyle` 查表实现 `draw*` / `pixelMetric` / `sizeFromContents` 等 |
| **Palette** | `standardPalette()` 由 Store Token 填充，覆盖未专门绘制的控件 |
| **支线** | 自绘控件经 `qtheme::api` 读同一 Store |
| **非目标（默认）** | Designer 插件接口、Accessible 实现细节、纯数据 Model（无绘制） |

**主题优先级（规划用，非覆盖状态）**

| 级 | 含义 | 典型 |
|----|------|------|
| **T0** | Style 绘制高频，必须进覆盖矩阵 | Button / Edit / Check / ScrollBar / TabBar / Menu / Header |
| **T1** | 常用，里程碑内逐步覆盖 | Slider / Progress / Combo / Spin / GroupBox / ToolBar / Views |
| **T2** | Palette 或基类回退可接受 | Label / LCD / 多数 Dialog 系统壳 / GPU 画布 |
| **T3** | 清单收录、默认不纳入引擎 | Graphics 图元生态、手势、Completer 等非视觉/非 Style 对象 |

---

## 1. 抽象基类

通常不直接实例化；决定子类度量与 Style 入口。

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QWidget` | 所有 UI 对象基类 | T0（通用 polish / palette） |
| `QDialog` | 对话框基类 | T1 |
| `QAbstractButton` | 按钮族 | T0 |
| `QAbstractSlider` | Slider / ScrollBar / Dial | T0–T1 |
| `QAbstractSpinBox` | Spin 族 | T1 |
| `QFrame` | 带框控件基类 | T1 |
| `QAbstractScrollArea` | 滚动视口基类 | T0 |
| `QAbstractItemView` | Model/View 视口基类 | T1 |
| `QLayout` / `QLayoutItem` | 布局抽象（见 §10） | T2（度量联动） |
| `QStyle` | 外观抽象（见 §11） | T0（引擎本体） |

---

## 2. 按钮与触发

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QPushButton` | 标准命令按钮 | T0 |
| `QToolButton` | 工具栏按钮 / 菜单弹出 | T0 |
| `QRadioButton` | 单选 | T0 |
| `QCheckBox` | 复选（含三态） | T0 |
| `QCommandLinkButton` | 带说明的命令链接按钮 | T1 |
| `QDialogButtonBox` | 对话框标准按钮组 | T1 |
| `QButtonGroup` | 非可视化编组 | T3 |

---

## 3. 文本与数值输入

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QLineEdit` | 单行编辑 | T0 |
| `QTextEdit` | 多行富文本 | T1 |
| `QPlainTextEdit` | 多行纯文本 | T1 |
| `QComboBox` | 下拉（可编辑） | T0 |
| `QFontComboBox` | 字体下拉 | T1 |
| `QSpinBox` / `QDoubleSpinBox` | 数值步进 | T1 |
| `QSlider` / `QDial` / `QScrollBar` | 范围调节 | T0–T1 |
| `QKeySequenceEdit` | 快捷键录入 | T2 |
| `QCompleter` | 非可视化补全引擎 | T3 |

---

## 4. 日期与时间

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QDateEdit` / `QTimeEdit` / `QDateTimeEdit` | 日期时间微调 | T1 |
| `QCalendarWidget` | 月历面板 | T1 |

---

## 5. 显示与画布

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QLabel` | 文本 / 图片标签 | T2（Palette） |
| `QTextBrowser` | 只读富文本 / 链接 | T2 |
| `QProgressBar` | 进度 / 繁忙 | T1 |
| `QLCDNumber` | 数码管样式数字 | T2 |
| `QUndoView` | 撤销栈列表（配合 Gui 的 `QUndoStack`） | T2 |
| `QGraphicsView` | Graphics View 视口（配套见 §12） | T2 |
| `QRhiWidget` | Widgets 内 RHI 画布（Qt 6.7+） | T3 |
| `QOpenGLWidget` | **OpenGLWidgets**：OpenGL 画布 | T3 |
| `QQuickWidget` | **QuickWidgets**：嵌入 QML | T3（非本引擎目标） |
| `QSvgWidget` | **SvgWidgets**：SVG 显示 | T3 |
| `QPrintPreviewWidget` | **PrintSupport**：嵌入式打印预览 | T3 |

---

## 6. 容器与页面组织

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QGroupBox` | 分组框（可 checkable） | T1 |
| `QScrollArea` | 滚动外壳 | T1 |
| `QFrame` | 线框 / 面板 | T1 |
| `QTabWidget` / `QTabBar` | 选项卡；**TabBar 为 Style 高频** | T0 |
| `QStackedWidget` / `QStackedLayout` | 堆叠页 / 堆叠布局 | T2 |
| `QToolBox` | 抽屉式折叠页 | T2 |
| `QDockWidget` | 停靠 / 浮动面板 | T1 |
| `QSplitter` / `QSplitterHandle` | 分割条与手柄 | T1 |
| `QMdiArea` / `QMdiSubWindow` | MDI 区域与子窗口 | T2 |
| `QFocusFrame` | 焦点框辅助 | T2 |
| `QRubberBand` | 橡皮筋选框 | T2 |
| `QSizeGrip` | 窗口缩放手柄 | T2 |

---

## 7. Model / View 与条目控件

### 7.1 Item-based（自带存储）

| 类 | 主题级 |
|----|--------|
| `QListWidget` / `QTreeWidget` / `QTableWidget` | T1 |
| `QListWidgetItem` / `QTreeWidgetItem` / `QTableWidgetItem` | T3（非 Widget） |

### 7.2 Model-based 视图

| 类 | 主题级 |
|----|--------|
| `QListView` / `QTreeView` / `QTableView` / `QColumnView` | T1 |
| `QHeaderView` | T0 |
| `QDataWidgetMapper` | T3 |

### 7.3 模型、选择与委托

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QStandardItemModel` / `QStringListModel` / `QFileSystemModel` 等 | 数据侧 | T3 |
| `QAbstractItemDelegate` / `QStyledItemDelegate` / `QItemDelegate` | 单元格绘制与就地编辑 | T1（与 View 协同） |
| `QItemSelectionModel` 等 | 选择状态 | T3 |
| `QItemEditorFactory` | 委托编辑器工厂 | T2 |

---

## 8. 主窗口与命令

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QMainWindow` | 主窗口骨架 | T1 |
| `QMenuBar` / `QMenu` | 菜单栏 / 菜单 | T0 |
| `QToolBar` | 工具栏 | T0 |
| `QStatusBar` | 状态栏 | T1 |
| `QAction` / `QActionGroup` | **Qt Gui**：命令抽象 | T3（外观落在 Menu/ToolButton） |
| `QWidgetAction` | 自定义控件型 Action | T2 |
| `QShortcut` | **Qt Gui**：快捷键 | T3 |

---

## 9. 标准对话框

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QDialog` | 自定义对话框基类 | T1 |
| `QMessageBox` / `QInputDialog` / `QErrorMessage` | 简易提示与输入 | T1（Palette + Button） |
| `QFileDialog` / `QFontDialog` / `QColorDialog` | 系统/标准选择器 | T2（平台原生时引擎影响有限） |
| `QProgressDialog` | 长任务进度对话框 | T1 |
| `QWizard` / `QWizardPage` | 向导 | T2 |
| `QPrintDialog` / `QPageSetupDialog` / `QPrintPreviewDialog` | **PrintSupport** | T3 |

---

## 10. 应用外壳与系统集成

| 类 | 说明 | 主题级 |
|----|------|--------|
| `QApplication` | `Engine::apply` 入口 | T0 |
| `QSplashScreen` | 启动闪屏 | T2 |
| `QSystemTrayIcon` | 托盘 | T3 |
| `QToolTip` | 工具提示 | T1 |
| `QWhatsThis` | 上下文帮助 | T3 |
| `QScroller` / `QScrollerProperties` | 惯性滚动 | T3 |
| `QGesture` / `QGestureRecognizer` | 手势 | T3 |
| `QFileIconProvider` | 文件系统图标 | T3 |

---

## 11. 布局

不直接走 `drawControl`，但 **margin / spacing / sizeHint** 与主题度量相关。

| 类 | 说明 |
|----|------|
| `QBoxLayout` / `QHBoxLayout` / `QVBoxLayout` | 线性布局 |
| `QGridLayout` / `QFormLayout` | 网格 / 表单 |
| `QStackedLayout` | 堆叠布局 |
| `QSpacerItem` / `QWidgetItem` / `QSizePolicy` | 空白、条目、尺寸策略 |

主题级：**T2**（通过 Style metric / 全局 spacing Token 间接影响）。

---

## 12. Style 与外观 API（引擎核心面）

本仓库 `QThemeStyle` 直接建立在此层之上。

| 类 | 说明 |
|----|------|
| `QStyle` | `drawControl` / `drawPrimitive` / `drawComplexControl` / `pixelMetric` / `sizeFromContents` / `subElementRect`… |
| `QCommonStyle` | 通用 Style 基类 |
| `QProxyStyle` | **本引擎基类策略**：按需覆盖，其余回退 Fusion 等 |
| `QStyleFactory` | 创建内置 Style |
| `QStyleOption`（及派生） | 绘制参数包 |
| `QStylePainter` / `QStylePlugin` | 辅助绘制 / 插件 |
| `QPalette` / `QFont` / `QFontMetrics` / `QColor`… | **Qt Gui**：色与字 |

---

## 13. Graphics View 配套

`QGraphicsView` 为视口；完整生态同属 Widgets，但 **默认非 QThemeEngine 主覆盖面**（T2/T3）。嵌入的真实 `QWidget`（经 `QGraphicsProxyWidget`）仍走 `QThemeStyle`。

| 分组 | 代表 |
|------|------|
| 场景 / 视口 | `QGraphicsScene`、`QGraphicsView` |
| 图元 | `QGraphicsItem`、`QGraphicsObject`、`QGraphicsWidget`、`QGraphicsProxyWidget`、常用 Shape/Text/Pixmap 图元 |
| 布局 / 特效 | `QGraphicsLinearLayout`、`QGraphicsGridLayout`、`QGraphicsAnchorLayout`、`QGraphicsEffect`、`QGraphicsTransform` |
| 邻接 | `QGraphicsSvgItem`（**Qt Svg**） |
| 事件 | `QGraphicsSceneEvent` 及派生（主题通常不碰） |

---

## 14. 模块边界

| 模块 | 与本仓库关系 |
|------|----------------|
| **Qt Widgets** | 主依赖；`QThemeStyle` 覆盖目标 |
| **Qt Gui** | Palette / Font / Action / 多数 Model；随 Widgets 传递 |
| **Qt OpenGLWidgets** / **PrintSupport** / **QuickWidgets** / **SvgWidgets** | Demo 可链；**非引擎必选依赖** |
| Qt Designer `QDesigner*Interface` | 不收录 |
| Accessible（如 `QAccessibleWidget`） | 不纳入主题清单 |

---

## 15. 维护约定

1. 新增「要对齐原生观感」的控件族时：先在本清单确认类名与模块，再在 [coverage-matrix.md](coverage-matrix.md) 增加行并实现 `QThemeStyle` 映射。  
2. 官方文档同一类可能出现在多个分类；本清单按**产品使用场景**归族。  
3. 示例画廊（`examples/native_controls`）用于肉眼验收，不替代覆盖矩阵状态。  
4. 上游 Qt 大版本若增删公开 Widget，以官方 Widget Classes 页为准更新本节。

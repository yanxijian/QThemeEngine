# Fluent Pack ↔ microsoft-ui-xaml 全量色审计

- Common: `D:\Codes\microsoft-ui-xaml\controls\dev\CommonStyles\Common_themeresources_any.xaml`
- QTE: `D:\Codes\QThemeEngine\resources\themes\fluent/fluent.{light,dark}.theme.json`
- 对照角色数（有映射）: 204
- QTE 角色总数: 204；无映射: 0

## 汇总

| 类别 | 数量 |
|------|------|
| OK / STRUCT_OK / ACCENT | 382 |
| CLOSE (≤24) | 12 |
| DIFF (严格语义偏差) | 0 |
| STRUCT_DIFF (半透明≈不透明偏差大) | 14 |
| QTE 无 WinUI 映射 | 0 |
| Common Color 未被任何映射引用 | 43 |
| 重要控件 StaticResource 未覆盖（含间接） | 11 |

图例：`OK` 通道差≤8；`CLOSE`≤24；`DIFF` 严格映射偏差；`STRUCT_*` 故意用不透明近似半透明；`ACCENT` 跟系统强调色样本比。

## 严格 DIFF / NO_REF

（无）

## STRUCT_DIFF（半透明近似偏差偏大）

| theme | qte | got | expect | Δ | flag | common |
|-------|-----|-----|--------|---|------|--------|
| dark | `combo/border` | `#3F3F3F` | `#9F9F9F` | 96 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| light | `combo/border` | `#D1D1D1` | `#8D8D8D` | 68 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| dark | `combo/border.hover` | `#6B6B6B` | `#9F9F9F` | 52 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| dark | `edit/border` | `#3F3F3F` | `#9F9F9F` | 96 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| light | `edit/border` | `#D1D1D1` | `#8D8D8D` | 68 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| dark | `edit/border.hover` | `#6B6B6B` | `#9F9F9F` | 52 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| light | `palette/dark` | `#8A8A8A` | `#EEEEEE` | 100 | STRUCT_DIFF | `SolidBackgroundFillColorSecondary` |
| dark | `slider/handle.border` | `#6B6B6B` | `#3B3B3B` | 48 | STRUCT_DIFF | `ControlStrokeColorDefault` |
| dark | `spin/border` | `#3F3F3F` | `#9F9F9F` | 96 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| light | `spin/border` | `#D1D1D1` | `#8D8D8D` | 68 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| dark | `spin/border.hover` | `#6B6B6B` | `#9F9F9F` | 52 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| dark | `textedit/border` | `#3F3F3F` | `#9F9F9F` | 96 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| light | `textedit/border` | `#D1D1D1` | `#8D8D8D` | 68 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |
| dark | `textedit/border.hover` | `#6B6B6B` | `#9F9F9F` | 52 | STRUCT_DIFF | `ControlStrongStrokeColorDefault` |

## CLOSE

| theme | qte | got | expect | Δ | flag | common |
|-------|-----|-----|--------|---|------|--------|
| light | `calendar/grid` | `#E0E0E0` | `#F0F0F0` | 16 | CLOSE | `DividerStrokeColorDefault` |
| light | `dock/title.bg` | `#EAEAEA` | `#F9F9F9` | 15 | CLOSE | `SolidBackgroundFillColorTertiary` |
| light | `header/border` | `#E0E0E0` | `#F0F0F0` | 16 | CLOSE | `DividerStrokeColorDefault` |
| light | `menu/separator` | `#E0E0E0` | `#F0F0F0` | 16 | CLOSE | `DividerStrokeColorDefault` |
| dark | `scroll/arrow` | `#B0B0B0` | `#9F9F9F` | 17 | CLOSE | `ControlStrongFillColorDefault` |
| dark | `scroll/arrow.disabled` | `#777777` | `#606060` | 23 | CLOSE | `ControlStrongFillColorDisabled` |
| light | `scroll/arrow.disabled` | `#A0A0A0` | `#AEAEAE` | 14 | CLOSE | `ControlStrongFillColorDisabled` |
| dark | `status/border` | `#3F3F3F` | `#323232` | 13 | CLOSE | `DividerStrokeColorDefault` |
| dark | `toolbar/border` | `#3F3F3F` | `#323232` | 13 | CLOSE | `DividerStrokeColorDefault` |
| dark | `toolbar/separator` | `#3F3F3F` | `#323232` | 13 | CLOSE | `DividerStrokeColorDefault` |
| light | `toolbar/separator` | `#D1D1D1` | `#E5E5E5` | 20 | CLOSE | `DividerStrokeColorDefault` |
| light | `view/grid` | `#E0E0E0` | `#F0F0F0` | 16 | CLOSE | `DividerStrokeColorDefault` |

## QTE 角色无映射

（无 — 全部角色已声明映射）

## 反向：Common Color 未被引用（可能遗漏 or 非本引擎范围）

下列是 `Common_themeresources_any.xaml` 的 `<Color>`，当前任何 QTE 映射都未指向它们。

### AccentTextFill* (1)

- `AccentTextFillColorDisabled`

### CardBackgroundFill* (2)

- `CardBackgroundFillColorSecondary`
- `CardBackgroundFillColorTertiary`

### CardStroke* (1)

- `CardStrokeColorDefaultSolid`

### ControlAltFill* (2)

- `ControlAltFillColorQuarternary`
- `ControlAltFillColorTransparent`

### ControlFill* (2)

- `ControlFillColorQuarternary`
- `ControlFillColorTransparent`

### ControlOnImageFill* (4)

- `ControlOnImageFillColorDefault`
- `ControlOnImageFillColorDisabled`
- `ControlOnImageFillColorSecondary`
- `ControlOnImageFillColorTertiary`

### ControlStroke* (6)

- `ControlStrokeColorForStrongFillWhenOnImage`
- `ControlStrokeColorOnAccentDefault`
- `ControlStrokeColorOnAccentDisabled`
- `ControlStrokeColorOnAccentSecondary`
- `ControlStrokeColorOnAccentTertiary`
- `ControlStrokeColorSecondary`

### LayerFill* (2)

- `LayerFillColorAlt`
- `LayerFillColorDefault`

### LayerOnAccentAcrylicFill* (1)

- `LayerOnAccentAcrylicFillColorDefault`

### LayerOnAcrylicFill* (1)

- `LayerOnAcrylicFillColorDefault`

### LayerOnMicaBaseAltFill* (3)

- `LayerOnMicaBaseAltFillColorSecondary`
- `LayerOnMicaBaseAltFillColorTertiary`
- `LayerOnMicaBaseAltFillColorTransparent`

### SmokeFill* (1)

- `SmokeFillColorDefault`

### SolidBackgroundFill* (3)

- `SolidBackgroundFillColorBaseAlt`
- `SolidBackgroundFillColorSenary`
- `SolidBackgroundFillColorTransparent`

### SurfaceStroke* (2)

- `SurfaceStrokeColorDefault`
- `SurfaceStrokeColorInverse`

### SystemFill* (10)

- `SystemFillColorAttentionBackground`
- `SystemFillColorCautionBackground`
- `SystemFillColorCriticalBackground`
- `SystemFillColorNeutral`
- `SystemFillColorNeutralBackground`
- `SystemFillColorSolidAttentionBackground`
- `SystemFillColorSolidNeutral`
- `SystemFillColorSolidNeutralBackground`
- `SystemFillColorSuccess`
- `SystemFillColorSuccessBackground`

### TextFill* (1)

- `TextFillColorInverse`

### TextOnAccentFill* (1)

- `TextOnAccentFillColorSelectedText`

## 反向：重要控件 StaticResource 未覆盖

这些控件资源在 WinUI 中存在，且未通过 QTE 映射直接/间接覆盖（间接=映射已用其目标 Common 色）。

| WinUI key | resolves to |
|-----------|-------------|
| `TextControlForegroundDisabled` | `TemporaryTextFillColorDisabled` |
| `TextControlBorderBrush` | `TextControlElevationBorder` |
| `TextControlBorderBrushFocused` | `TextControlElevationBorderFocused` |
| `TextControlSelectionHighlightColor` | `AccentFillColorSelectedTextBackground` |
| `ButtonBorderBrush` | `ControlElevationBorder` |
| `ComboBoxPlaceholderTextForegroundThemeBrush` | `?` |
| `ComboBoxBorderBrushFocused` | `?` |
| `MenuFlyoutPresenterBackground` | `DesktopAcrylicTransparent` |
| `AppBarBackground` | `?` |
| `ToolTipBackground` | `SystemControlBackgroundChromeMediumLow` |
| `CalendarViewSelectedBackground` | `?` |

## 建议优先补的遗漏（人工筛选）

- **ControlFillColorInputActive** — TextBox 聚焦底 — textedit/bg 已 STRUCT 映射
- **SystemFillColorSuccess** — 成功态 — QTE 暂无独立 role（Caution/Critical 已映 progress.chunk.paused|error）
- **MenuFlyoutPresenterBackground** — 真 Acrylic 需平台材质；现 menu/bg.acrylic 为实色近似
- **TextOnAccentFillColorDisabled** — Accent 禁用文字 — check/indicator.disabled 已 STRUCT

## 结论（本轮）

1. **正向**：全部 QTE 色 role 已声明 WinUI 映射；严格 `DIFF=0`。剩余 `STRUCT_DIFF` 主要为半透明描边/填充的不透明近似、以及焦点环内外描边。
2. **本轮已补**：Accent Secondary/Tertiary、TextOnAccentSecondary、Progress paused/error（`qtheme.progressState`）、Focus 双描边、view selected+hover、text.tertiary、menu/bg.acrylic、combo placeholder。
3. **仍非本引擎范围**：真 Acrylic/Mica 材质、SystemFill Success 独立态。

复跑：`python scripts/audit_fluent_vs_mux_full.py`


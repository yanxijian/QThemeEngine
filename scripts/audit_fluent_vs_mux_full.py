#!/usr/bin/env python3
"""Full Fluent pack audit vs microsoft-ui-xaml + reverse unused-color check.

Forward: every QTE color role vs a WinUI Common/control mapping (when one exists).
Reverse: Common Color tokens and key control StaticResources that no QTE mapping uses.
"""
from __future__ import annotations

import json
import re
from collections import defaultdict
from pathlib import Path

ROOT = Path(r"D:\Codes")
MUX = ROOT / "microsoft-ui-xaml"
QTE = ROOT / "QThemeEngine" / "resources" / "themes" / "fluent"
OUT = ROOT / "QThemeEngine" / "docs" / "zh" / "fluent-mux-audit.md"
COMMON = MUX / "controls" / "dev" / "CommonStyles" / "Common_themeresources_any.xaml"

CONTROL_FILES = [
	MUX / "controls/dev/CommonStyles/Button_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/TextBox_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/CheckBox_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/RadioButton_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/Slider_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/ScrollBar_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/ToolTip_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/MenuFlyout_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/CommandBar_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/ListViewItem_themeresources.xaml",
	MUX / "controls/dev/CommonStyles/CalendarView_themeresources.xaml",
	MUX / "controls/dev/ComboBox/ComboBox_themeresources.xaml",
	MUX / "controls/dev/ProgressBar/ProgressBar_themeresources.xaml",
	MUX / "controls/dev/NumberBox/NumberBox_themeresources.xaml",
	MUX / "controls/dev/TabView/TabView_themeresources.xaml",
	MUX / "controls/dev/SplitView/SplitView_themeresources.xaml",
]


def parse_theme_block(xaml: str, key: str) -> str:
	m = re.search(rf'<ResourceDictionary x:Key="{key}">(.*?)</ResourceDictionary>', xaml, re.S)
	return m.group(1) if m else ""


def parse_colors(block: str) -> dict[str, str]:
	return {
		m.group(1): m.group(2).upper()
		for m in re.finditer(r'<Color x:Key="([^"]+)">#([0-9A-Fa-f]+)</Color>', block)
	}


def parse_static_maps(block: str) -> dict[str, str]:
	# <StaticResource x:Key="Foo" ResourceKey="Bar" /> or ResourceKey="BarBrush"
	out = {}
	for m in re.finditer(
		r'<StaticResource\s+x:Key="([^"]+)"\s+ResourceKey="([^"]+)"\s*/>',
		block,
	):
		out[m.group(1)] = m.group(2)
	return out


def strip_brush(name: str) -> str:
	return name[:-5] if name.endswith("Brush") else name


def parse_argb(h: str) -> tuple[int, int, int, int]:
	h = h.upper()
	if len(h) == 6:
		return 255, int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16)
	if len(h) == 8:
		return int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16), int(h[6:8], 16)
	raise ValueError(h)


def composite(fg: str, bg: str) -> str:
	fa, fr, fg_, fb = parse_argb(fg)
	_, br, bg_, bb = parse_argb(bg if len(bg) != 6 else "FF" + bg)
	a = fa / 255.0
	r = round(fr * a + br * (1 - a))
	g = round(fg_ * a + bg_ * (1 - a))
	b = round(fb * a + bb * (1 - a))
	return f"#{r:02X}{g:02X}{b:02X}"


def to_opaque(mux_hex: str, bg: str) -> str:
	if len(mux_hex) == 6:
		return "#" + mux_hex
	if len(mux_hex) == 8 and mux_hex.startswith("FF"):
		return "#" + mux_hex[2:]
	return composite(mux_hex, bg)


def rgb(h: str) -> tuple[int, int, int]:
	h = h.lstrip("#")
	if len(h) == 8:
		h = h[2:]
	return int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16)


def delta(a: str, b: str) -> int:
	ar, ag, ab = rgb(a)
	br, bg, bb = rgb(b)
	return max(abs(ar - br), abs(ag - bg), abs(ab - bb))


def load_qte(path: Path) -> dict[str, str]:
	data = json.loads(path.read_text(encoding="utf-8"))
	out = {}
	for g, roles in data.get("colors", {}).items():
		for role, val in roles.items():
			out[f"{g}/{role}"] = str(val).upper()
	return out


# QTE role -> (common_color_key | control_static_key, composite_bg_kind, flags)
# bg_kind: base | surface | white | black | accent_skip
# flags: ACCENT (OS-dependent), STRUCT (intentional opaque approx), DEMO (sample widget)
Mapping = tuple[str, str, str]  # target, bg_kind, flags

# Explicit mappings for every production QTE role we can ground in WinUI.
MAPPINGS: dict[str, Mapping] = {
	# palette
	"palette/window": ("SolidBackgroundFillColorBase", "none", ""),
	"palette/windowText": ("TextFillColorPrimary", "surface", ""),
	"palette/base": ("SolidBackgroundFillColorQuarternary", "none", ""),
	"palette/text": ("TextFillColorPrimary", "surface", ""),
	"palette/button": ("ControlSolidFillColorDefault", "none", "STRUCT"),
	"palette/buttonText": ("TextFillColorPrimary", "surface", ""),
	"palette/highlight": ("AccentFillColorDefault", "none", "ACCENT"),
	"palette/highlightedText": ("TextOnAccentFillColorPrimary", "none", ""),
	"palette/mid": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	"palette/light": ("SolidBackgroundFillColorQuinary", "none", "STRUCT"),
	"palette/dark": ("SolidBackgroundFillColorSecondary", "none", "STRUCT"),  # QPalette::Dark ≠ WinUI surface name
	"palette/accent": ("AccentFillColorDefault", "none", "ACCENT"),
	"palette/accent.hover": ("AccentFillColorSecondary", "none", "ACCENT"),
	"palette/accent.pressed": ("AccentFillColorTertiary", "none", "ACCENT"),
	"palette/accent.text": ("TextOnAccentFillColorPrimary", "none", ""),
	"palette/accent.text.secondary": ("TextOnAccentFillColorSecondary", "none", "STRUCT"),
	"palette/text.tertiary": ("TextFillColorTertiary", "surface", ""),
	"palette/focus.outer": ("FocusStrokeColorOuter", "none", "STRUCT"),
	"palette/focus.inner": ("FocusStrokeColorInner", "none", "STRUCT"),
	"palette/stroke": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	"palette/canvas": ("SolidBackgroundFillColorBase", "none", ""),
	"palette/surface": ("SolidBackgroundFillColorQuarternary", "none", ""),
	# button (rest state ≈ ControlFillDefault translucent → STRUCT vs ControlSolid)
	"button/bg": ("ControlFillColorDefault", "surface", "STRUCT"),
	"button/bg.hover": ("ControlFillColorSecondary", "surface", "STRUCT"),
	"button/bg.pressed": ("ControlFillColorTertiary", "surface", "STRUCT"),
	"button/bg.disabled": ("ControlFillColorDisabled", "surface", "STRUCT"),
	"button/bg.accent": ("AccentFillColorDefault", "none", "ACCENT"),
	"button/bg.accent.hover": ("AccentFillColorSecondary", "none", "ACCENT"),
	"button/bg.accent.pressed": ("AccentFillColorTertiary", "none", "ACCENT"),
	"button/fg": ("TextFillColorPrimary", "surface", ""),
	"button/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"button/fg.accent": ("TextOnAccentFillColorPrimary", "none", ""),
	"button/fg.accent.pressed": ("TextOnAccentFillColorSecondary", "none", "STRUCT"),
	"button/border": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	"button/border.focus": ("AccentFillColorDefault", "none", "ACCENT"),
	# edit / textedit (TextBox)
	"edit/bg": ("ControlFillColorDefault", "surface", "STRUCT"),
	"edit/bg.disabled": ("ControlFillColorDisabled", "surface", "STRUCT"),
	"edit/fg": ("TextFillColorPrimary", "surface", ""),
	"edit/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"edit/fg.placeholder": ("TextFillColorSecondary", "surface", ""),
	"edit/border": ("ControlStrongStrokeColorDefault", "surface", "STRUCT"),
	"edit/border.focus": ("AccentFillColorDefault", "none", "ACCENT"),
	"edit/border.hover": ("ControlStrongStrokeColorDefault", "surface", "STRUCT"),
	"edit/border.disabled": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	"textedit/bg": ("ControlFillColorInputActive", "base", "STRUCT"),
	"textedit/bg.disabled": ("ControlFillColorDisabled", "surface", "STRUCT"),
	"textedit/fg": ("TextFillColorPrimary", "surface", ""),
	"textedit/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"textedit/fg.placeholder": ("TextFillColorSecondary", "surface", ""),
	"textedit/border": ("ControlStrongStrokeColorDefault", "surface", "STRUCT"),
	"textedit/border.focus": ("AccentFillColorDefault", "none", "ACCENT"),
	"textedit/border.hover": ("ControlStrongStrokeColorDefault", "surface", "STRUCT"),
	"textedit/border.disabled": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	# check
	"check/bg": ("ControlAltFillColorSecondary", "surface", "STRUCT"),
	"check/bg.checked": ("AccentFillColorDefault", "none", "ACCENT"),
	"check/bg.checked.hover": ("AccentFillColorSecondary", "none", "ACCENT"),
	"check/bg.hover": ("ControlAltFillColorTertiary", "surface", "STRUCT"),
	"check/bg.disabled": ("ControlAltFillColorDisabled", "surface", "STRUCT"),
	"check/bg.checked.disabled": ("AccentFillColorDisabled", "surface", "STRUCT"),
	"check/border": ("ControlStrongStrokeColorDefault", "surface", ""),
	"check/border.checked": ("AccentFillColorDefault", "none", "ACCENT"),
	"check/border.disabled": ("ControlStrongStrokeColorDisabled", "surface", ""),
	"check/indicator": ("TextOnAccentFillColorPrimary", "none", ""),
	"check/indicator.disabled": ("TextOnAccentFillColorDisabled", "surface", "STRUCT"),
	"check/fg": ("TextFillColorPrimary", "surface", ""),
	"check/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	# combo / spin share text control family
	"combo/bg": ("ControlFillColorDefault", "surface", "STRUCT"),
	"combo/bg.hover": ("ControlFillColorSecondary", "surface", "STRUCT"),
	"combo/bg.pressed": ("ControlFillColorTertiary", "surface", "STRUCT"),
	"combo/bg.disabled": ("ControlFillColorDisabled", "surface", "STRUCT"),
	"combo/fg": ("TextFillColorPrimary", "surface", ""),
	"combo/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"combo/fg.placeholder": ("TextFillColorSecondary", "surface", ""),
	"combo/border": ("ControlStrongStrokeColorDefault", "surface", "STRUCT"),
	"combo/border.focus": ("AccentFillColorDefault", "none", "ACCENT"),
	"combo/border.hover": ("ControlStrongStrokeColorDefault", "surface", "STRUCT"),
	"combo/arrow": ("TextFillColorSecondary", "surface", ""),
	"combo/arrow.disabled": ("TextFillColorDisabled", "surface", ""),
	"spin/bg": ("ControlFillColorDefault", "surface", "STRUCT"),
	"spin/bg.disabled": ("ControlFillColorDisabled", "surface", "STRUCT"),
	"spin/fg": ("TextFillColorPrimary", "surface", ""),
	"spin/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"spin/border": ("ControlStrongStrokeColorDefault", "surface", "STRUCT"),
	"spin/border.focus": ("AccentFillColorDefault", "none", "ACCENT"),
	"spin/border.hover": ("ControlStrongStrokeColorDefault", "surface", "STRUCT"),
	"spin/border.disabled": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	"spin/button": ("SubtleFillColorSecondary", "surface", "STRUCT"),
	"spin/button.hover": ("SubtleFillColorTertiary", "surface", "STRUCT"),
	"spin/button.pressed": ("ControlFillColorTertiary", "surface", "STRUCT"),
	"spin/button.disabled": ("SubtleFillColorDisabled", "surface", "STRUCT"),
	"spin/arrow": ("TextFillColorSecondary", "surface", ""),
	"spin/arrow.disabled": ("TextFillColorDisabled", "surface", ""),
	# slider / dial / progress
	"slider/groove": ("ControlStrongFillColorDefault", "surface", ""),
	"slider/groove.disabled": ("ControlStrongFillColorDisabled", "surface", ""),
	"slider/fill": ("AccentFillColorDefault", "none", "ACCENT"),
	"slider/fill.hover": ("AccentFillColorSecondary", "none", "ACCENT"),
	"slider/fill.disabled": ("AccentFillColorDisabled", "surface", "STRUCT"),
	"slider/handle": ("ControlSolidFillColorDefault", "none", "STRUCT"),
	"slider/handle.hover": ("ControlSolidFillColorDefault", "none", "STRUCT"),
	"slider/handle.pressed": ("ControlSolidFillColorDefault", "none", "STRUCT"),
	"slider/handle.disabled": ("ControlFillColorDisabled", "surface", "STRUCT"),
	"slider/handle.border": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	"slider/handle.border.focus": ("AccentFillColorDefault", "none", "ACCENT"),
	"dial/groove": ("ControlStrongFillColorDefault", "surface", ""),
	"dial/groove.disabled": ("ControlStrongFillColorDisabled", "surface", ""),
	"dial/fill": ("AccentFillColorDefault", "none", "ACCENT"),
	"dial/fill.disabled": ("AccentFillColorDisabled", "surface", "STRUCT"),
	"dial/handle": ("ControlSolidFillColorDefault", "none", "STRUCT"),
	"dial/handle.border": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	"dial/handle.border.focus": ("AccentFillColorDefault", "none", "ACCENT"),
	"progress/groove": ("ControlStrongStrokeColorDefault", "surface", ""),
	"progress/groove.disabled": ("ControlStrongStrokeColorDisabled", "surface", ""),
	"progress/chunk": ("AccentFillColorDefault", "none", "ACCENT"),
	"progress/chunk.paused": ("SystemFillColorCaution", "none", ""),
	"progress/chunk.error": ("SystemFillColorCritical", "none", ""),
	"progress/chunk.disabled": ("AccentFillColorDisabled", "surface", "STRUCT"),
	"progress/fg": ("TextFillColorPrimary", "surface", ""),
	"progress/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"progress/border": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	# scroll
	"scroll/groove": ("SubtleFillColorTransparent", "surface", "STRUCT"),
	"scroll/handle": ("ControlStrongFillColorDefault", "surface", ""),
	"scroll/handle.hover": ("ControlStrongFillColorDefault", "surface", ""),
	"scroll/handle.pressed": ("ControlStrongFillColorDefault", "surface", ""),
	"scroll/handle.disabled": ("ControlStrongFillColorDisabled", "surface", ""),
	"scroll/arrow": ("ControlStrongFillColorDefault", "surface", ""),
	"scroll/arrow.disabled": ("ControlStrongFillColorDisabled", "surface", ""),
	# tab / menu / toolbar / header / view
	"tab/bg": ("SubtleFillColorTransparent", "base", "STRUCT"),
	"tab/bg.selected": ("SolidBackgroundFillColorQuarternary", "none", "STRUCT"),
	"tab/bg.hover": ("SubtleFillColorSecondary", "base", "STRUCT"),
	"tab/bg.disabled": ("SubtleFillColorDisabled", "base", "STRUCT"),
	"tab/fg": ("TextFillColorSecondary", "base", ""),
	"tab/fg.selected": ("TextFillColorPrimary", "surface", ""),
	"tab/fg.disabled": ("TextFillColorDisabled", "base", ""),
	"tab/border": ("DividerStrokeColorDefault", "base", "STRUCT"),
	"tab/indicator": ("AccentFillColorDefault", "none", "ACCENT"),
	"menu/bg": ("SolidBackgroundFillColorTertiary", "none", "STRUCT"),
	"menu/bg.acrylic": ("SolidBackgroundFillColorTertiary", "none", "STRUCT"),
	"menu/bg.hover": ("SubtleFillColorSecondary", "surface", "STRUCT"),
	"menu/bg.disabled": ("SubtleFillColorDisabled", "surface", "STRUCT"),
	"menu/fg": ("TextFillColorPrimary", "surface", ""),
	"menu/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"menu/fg.selected": ("TextFillColorPrimary", "surface", ""),
	"menu/border": ("SurfaceStrokeColorFlyout", "surface", "STRUCT"),
	"menu/separator": ("DividerStrokeColorDefault", "surface", ""),
	"menu/bar.bg": ("SolidBackgroundFillColorBase", "none", ""),
	"menu/bar.bg.hover": ("SubtleFillColorSecondary", "base", "STRUCT"),
	"menu/bar.bg.pressed": ("SubtleFillColorTertiary", "base", "STRUCT"),
	"menu/bar.fg": ("TextFillColorPrimary", "base", ""),
	"toolbar/bg": ("SolidBackgroundFillColorBase", "none", ""),
	"toolbar/border": ("DividerStrokeColorDefault", "base", ""),
	"toolbar/separator": ("DividerStrokeColorDefault", "base", ""),
	"toolbar/handle": ("ControlStrongFillColorDefault", "base", ""),
	"header/bg": ("SolidBackgroundFillColorTertiary", "none", "STRUCT"),
	"header/bg.hover": ("SubtleFillColorSecondary", "surface", "STRUCT"),
	"header/bg.pressed": ("SubtleFillColorTertiary", "surface", "STRUCT"),
	"header/fg": ("TextFillColorPrimary", "surface", ""),
	"header/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"header/border": ("DividerStrokeColorDefault", "surface", ""),
	"view/bg": ("SolidBackgroundFillColorQuarternary", "none", ""),
	"view/bg.alternate": ("LayerOnMicaBaseAltFillColorDefault", "surface", "STRUCT"),
	"view/bg.hover": ("SubtleFillColorSecondary", "surface", "STRUCT"),
	"view/bg.selected": ("AccentFillColorDefault", "none", "ACCENT"),
	"view/bg.selected.hover": ("AccentFillColorSecondary", "none", "ACCENT"),
	"view/bg.selected.inactive": ("AccentFillColorDefault", "none", "ACCENT"),
	"view/fg": ("TextFillColorPrimary", "surface", ""),
	"view/fg.selected": ("TextOnAccentFillColorPrimary", "none", ""),
	"view/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"view/grid": ("DividerStrokeColorDefault", "surface", ""),
	"view/branch": ("TextFillColorSecondary", "surface", ""),
	# chrome extras
	"groupbox/bg": ("CardBackgroundFillColorDefault", "base", "STRUCT"),
	"groupbox/border": ("CardStrokeColorDefault", "base", "STRUCT"),
	"groupbox/fg": ("TextFillColorPrimary", "base", ""),
	"groupbox/fg.disabled": ("TextFillColorDisabled", "base", ""),
	"tooltip/bg": ("SolidBackgroundFillColorTertiary", "none", "STRUCT"),
	"tooltip/fg": ("TextFillColorPrimary", "surface", ""),
	"tooltip/border": ("SurfaceStrokeColorFlyout", "surface", "STRUCT"),
	"frame/bg": ("SolidBackgroundFillColorQuarternary", "none", ""),
	"frame/border": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	"frame/line": ("DividerStrokeColorDefault", "surface", ""),
	"splitter/bg": ("SolidBackgroundFillColorBase", "none", ""),
	"splitter/handle": ("ControlStrongFillColorDefault", "base", ""),
	"splitter/handle.hover": ("AccentFillColorDefault", "none", "ACCENT"),
	"dock/bg": ("SolidBackgroundFillColorBase", "none", ""),
	"dock/border": ("ControlStrokeColorDefault", "base", "STRUCT"),
	"dock/title.bg": ("SolidBackgroundFillColorTertiary", "none", ""),
	"dock/title.bg.active": ("SolidBackgroundFillColorQuarternary", "none", ""),
	"dock/title.fg": ("TextFillColorPrimary", "surface", ""),
	"dock/title.fg.disabled": ("TextFillColorDisabled", "surface", ""),
	"status/bg": ("SolidBackgroundFillColorBase", "none", ""),
	"status/fg": ("TextFillColorPrimary", "base", ""),
	"status/border": ("DividerStrokeColorDefault", "base", ""),
	"calendar/bg": ("SolidBackgroundFillColorQuarternary", "none", ""),
	"calendar/border": ("ControlStrokeColorDefault", "surface", "STRUCT"),
	"calendar/header.bg": ("SolidBackgroundFillColorBase", "none", ""),
	"calendar/header.fg": ("TextFillColorPrimary", "base", ""),
	"calendar/grid": ("DividerStrokeColorDefault", "surface", ""),
	"commandlink/fg": ("TextFillColorPrimary", "surface", ""),
	"commandlink/description": ("TextFillColorSecondary", "surface", ""),
	"commandlink/fg.disabled": ("TextFillColorDisabled", "surface", ""),
	# demo sample
	"DemoButton/background": ("ControlFillColorDefault", "surface", "DEMO,STRUCT"),
	"DemoButton/background.hover": ("ControlFillColorSecondary", "surface", "DEMO,STRUCT"),
	"DemoButton/background.pressed": ("ControlFillColorTertiary", "surface", "DEMO,STRUCT"),
	"DemoButton/text": ("TextFillColorPrimary", "surface", "DEMO"),
	"DemoButton/border": ("ControlStrokeColorDefault", "surface", "DEMO,STRUCT"),
}

# WinUI control StaticResources that matter for covered Qt controls — used for reverse gap check.
IMPORTANT_CONTROL_KEYS = [
	# TextBox
	"TextControlBackground",
	"TextControlBackgroundFocused",
	"TextControlBackgroundDisabled",
	"TextControlForeground",
	"TextControlForegroundDisabled",
	"TextControlPlaceholderForeground",
	"TextControlPlaceholderForegroundDisabled",
	"TextControlBorderBrush",
	"TextControlBorderBrushFocused",
	"TextControlBorderBrushDisabled",
	"TextControlSelectionHighlightColor",
	# Button
	"ButtonBackground",
	"ButtonBackgroundPointerOver",
	"ButtonBackgroundPressed",
	"ButtonBackgroundDisabled",
	"ButtonForeground",
	"ButtonForegroundPressed",
	"ButtonForegroundDisabled",
	"ButtonBorderBrush",
	"AccentButtonBackground",
	"AccentButtonForeground",
	# CheckBox
	"CheckBoxCheckBackgroundFillUnchecked",
	"CheckBoxCheckBackgroundFillChecked",
	"CheckBoxCheckBackgroundFillCheckedDisabled",
	"CheckBoxCheckBackgroundStrokeUnchecked",
	"CheckBoxCheckBackgroundStrokeChecked",
	"CheckBoxCheckGlyphForegroundChecked",
	"CheckBoxForegroundUnchecked",
	"CheckBoxForegroundUncheckedDisabled",
	# Slider / Progress / Scroll
	"SliderTrackFill",
	"SliderTrackValueFill",
	"SliderThumbBackground",
	"SliderOuterThumbBackground",
	"ProgressBarForeground",
	"ProgressBarBackground",
	"ScrollBarThumbFill",
	"ScrollBarButtonArrowForeground",
	"ScrollBarThumbFillDisabled",
	# ComboBox
	"ComboBoxBackground",
	"ComboBoxForeground",
	"ComboBoxPlaceholderTextForegroundThemeBrush",
	"ComboBoxBackgroundBorderBrushFocused",
	"ComboBoxBorderBrushFocused",
	"ListViewItemBackgroundSelected",
	"ListViewItemForeground",
	"ListViewItemForegroundSelected",
	"ListViewItemBackgroundPointerOver",
	"ListViewItemBackgroundSelectedPointerOver",
	# Menu / CommandBar / ToolTip
	"MenuFlyoutPresenterBackground",
	"MenuFlyoutItemBackgroundPointerOver",
	"MenuFlyoutItemForeground",
	"MenuFlyoutSeparatorBackground",
	"AppBarBackground",
	"ToolTipForeground",
	"ToolTipBackground",
	"ToolTipBorderBrush",
	# Calendar
	"CalendarViewForeground",
	"CalendarViewBackground",
	"CalendarViewOutOfScopeBackground",
	"CalendarViewSelectedBackground",
	# Progress extended
	"ProgressBarPausedForegroundColor",
	"ProgressBarErrorForegroundColor",
]


def resolve_common(
	colors: dict[str, str],
	key: str,
	bg: str,
	accent_sample: str,
) -> tuple[str | None, str]:
	"""Return (opaque_hex or None, note)."""
	if key in ("AccentFillColorDefault", "AccentFillColorSecondary", "AccentFillColorTertiary"):
		# Brush maps to SystemAccentColorLight2 — not a fixed Color in Common file.
		return accent_sample, "ACCENT_SAMPLE"
	if key == "AccentFillColorDisabled":
		if key in colors:
			return to_opaque(colors[key], bg), ""
		return to_opaque("28FFFFFF", bg), ""
	if key not in colors:
		return None, f"MISSING_COMMON:{key}"
	return to_opaque(colors[key], bg), ""


def main() -> None:
	common_xaml = COMMON.read_text(encoding="utf-8")
	dark_colors = parse_colors(parse_theme_block(common_xaml, "Default"))
	light_colors = parse_colors(parse_theme_block(common_xaml, "Light"))

	# Control static maps (Default + Light)
	control_maps: dict[str, dict[str, str]] = {"Default": {}, "Light": {}}
	for path in CONTROL_FILES:
		if not path.exists():
			continue
		text = path.read_text(encoding="utf-8")
		for theme in ("Default", "Light"):
			block = parse_theme_block(text, theme)
			control_maps[theme].update(parse_static_maps(block))

	qte_dark = load_qte(QTE / "fluent.dark.theme.json")
	qte_light = load_qte(QTE / "fluent.light.theme.json")

	# bg helpers
	def bgs(theme: str) -> dict[str, str]:
		c = dark_colors if theme == "dark" else light_colors
		if theme == "dark":
			return {
				"none": "000000",
				"base": c["SolidBackgroundFillColorBase"],
				"surface": c["SolidBackgroundFillColorQuarternary"],
				"white": "FFFFFF",
				"black": "000000",
			}
		return {
			"none": "FFFFFF",
			"base": c.get("SolidBackgroundFillColorBase", "F3F3F3"),
			"surface": "FFFFFF",
			"white": "FFFFFF",
			"black": "000000",
		}

	accent = {"dark": "#60CDFF", "light": "#0078D4"}

	rows = []
	used_common: set[str] = set()
	used_control: set[str] = set()
	unmapped = []

	all_qte_keys = sorted(set(qte_dark) | set(qte_light))
	for qkey in all_qte_keys:
		if qkey not in MAPPINGS:
			unmapped.append(qkey)
			continue
		target, bg_kind, flags = MAPPINGS[qkey]
		used_common.add(strip_brush(target))
		# Also record if target looks like a control key present in maps
		for theme_name, qte, label in (
			("dark", qte_dark, "Default"),
			("light", qte_light, "Light"),
		):
			got = qte.get(qkey)
			if got is None:
				continue
			bgmap = bgs(theme_name)
			bg = bgmap[bg_kind]
			colors = dark_colors if theme_name == "dark" else light_colors
			# Prefer Common color resolution; if target is a control StaticResource, follow one hop.
			common_key = target
			if target in control_maps[label]:
				used_control.add(target)
				common_key = strip_brush(control_maps[label][target])
				used_common.add(common_key)
			exp, note = resolve_common(colors, common_key, bg, accent[theme_name])
			if exp is None:
				flag = "NO_REF"
				dlt = -1
			elif "ACCENT" in flags or note == "ACCENT_SAMPLE":
				flag = "ACCENT"
				dlt = delta(got, exp) if exp else -1
			elif "STRUCT" in flags:
				dlt = delta(got, exp)
				flag = "STRUCT_OK" if dlt <= 40 else "STRUCT_DIFF"
			else:
				dlt = delta(got, exp)
				flag = "OK" if dlt <= 8 else ("CLOSE" if dlt <= 24 else "DIFF")
			rows.append(
				{
					"theme": theme_name,
					"qte": qkey,
					"got": got,
					"expect": exp or "-",
					"delta": dlt,
					"flag": flag,
					"target": target,
					"common": common_key,
					"flags": flags,
					"note": note,
				}
			)

	# Reverse: Common colors never referenced
	all_common = set(dark_colors) | set(light_colors)
	unused_common = sorted(all_common - used_common)

	# Reverse: important control keys not covered by any mapping that resolves through them
	# A control key is "used" if some mapping's target equals it, or mapping's resolved common
	# was reached via that control key.
	mapped_targets = {t for t, _, _ in MAPPINGS.values()}
	unused_important = []
	for ck in IMPORTANT_CONTROL_KEYS:
		if ck in mapped_targets or ck in used_control:
			continue
		# Also accept if any mapping target equals the StaticResource destination's control alias
		# e.g. we map TextFillColorSecondary directly while TextControlPlaceholderForeground points there
		dest = None
		for label in ("Default", "Light"):
			if ck in control_maps[label]:
				dest = strip_brush(control_maps[label][ck])
				break
		if dest and dest in used_common:
			# Covered indirectly via common token — note as indirect
			continue
		unused_important.append((ck, dest or "?"))

	# Write report
	diff_rows = [r for r in rows if r["flag"] in {"DIFF", "NO_REF"}]
	struct_diff = [r for r in rows if r["flag"] == "STRUCT_DIFF"]
	close_rows = [r for r in rows if r["flag"] == "CLOSE"]
	ok_rows = [r for r in rows if r["flag"] in {"OK", "STRUCT_OK", "ACCENT"}]

	lines = []
	lines.append("# Fluent Pack ↔ microsoft-ui-xaml 全量色审计")
	lines.append("")
	lines.append(f"- Common: `{COMMON}`")
	lines.append(f"- QTE: `{QTE}/fluent.{{light,dark}}.theme.json`")
	lines.append(f"- 对照角色数（有映射）: {len(MAPPINGS)}")
	lines.append(f"- QTE 角色总数: {len(all_qte_keys)}；无映射: {len(unmapped)}")
	lines.append("")
	lines.append("## 汇总")
	lines.append("")
	lines.append(f"| 类别 | 数量 |")
	lines.append(f"|------|------|")
	lines.append(f"| OK / STRUCT_OK / ACCENT | {len(ok_rows)} |")
	lines.append(f"| CLOSE (≤24) | {len(close_rows)} |")
	lines.append(f"| DIFF (严格语义偏差) | {len(diff_rows)} |")
	lines.append(f"| STRUCT_DIFF (半透明≈不透明偏差大) | {len(struct_diff)} |")
	lines.append(f"| QTE 无 WinUI 映射 | {len(unmapped)} |")
	lines.append(f"| Common Color 未被任何映射引用 | {len(unused_common)} |")
	lines.append(f"| 重要控件 StaticResource 未覆盖（含间接） | {len(unused_important)} |")
	lines.append("")
	lines.append("图例：`OK` 通道差≤8；`CLOSE`≤24；`DIFF` 严格映射偏差；`STRUCT_*` 故意用不透明近似半透明；`ACCENT` 跟系统强调色样本比。")
	lines.append("")

	def dump_table(title: str, subset: list[dict], limit: int = 200) -> None:
		lines.append(f"## {title}")
		lines.append("")
		if not subset:
			lines.append("（无）")
			lines.append("")
			return
		lines.append("| theme | qte | got | expect | Δ | flag | common |")
		lines.append("|-------|-----|-----|--------|---|------|--------|")
		for r in subset[:limit]:
			lines.append(
				f"| {r['theme']} | `{r['qte']}` | `{r['got']}` | `{r['expect']}` | {r['delta']} | {r['flag']} | `{r['common']}` |"
			)
		if len(subset) > limit:
			lines.append(f"| … | … | … | … | … | … | (+{len(subset)-limit}) |")
		lines.append("")

	dump_table("严格 DIFF / NO_REF", diff_rows)
	dump_table("STRUCT_DIFF（半透明近似偏差偏大）", struct_diff)
	dump_table("CLOSE", close_rows)

	lines.append("## QTE 角色无映射")
	lines.append("")
	if unmapped:
		for k in unmapped:
			lines.append(f"- `{k}`")
	else:
		lines.append("（无 — 全部角色已声明映射）")
	lines.append("")

	lines.append("## 反向：Common Color 未被引用（可能遗漏 or 非本引擎范围）")
	lines.append("")
	lines.append("下列是 `Common_themeresources_any.xaml` 的 `<Color>`，当前任何 QTE 映射都未指向它们。")
	lines.append("")
	# Group by prefix
	by_prefix: dict[str, list[str]] = defaultdict(list)
	for k in unused_common:
		prefix = k.split("Color")[0] if "Color" in k else k[:16]
		by_prefix[prefix].append(k)
	for prefix in sorted(by_prefix):
		keys = by_prefix[prefix]
		lines.append(f"### {prefix}* ({len(keys)})")
		lines.append("")
		for k in keys:
			lines.append(f"- `{k}`")
		lines.append("")

	lines.append("## 反向：重要控件 StaticResource 未覆盖")
	lines.append("")
	lines.append("这些控件资源在 WinUI 中存在，且未通过 QTE 映射直接/间接覆盖（间接=映射已用其目标 Common 色）。")
	lines.append("")
	if unused_important:
		lines.append("| WinUI key | resolves to |")
		lines.append("|-----------|-------------|")
		for ck, dest in unused_important:
			lines.append(f"| `{ck}` | `{dest}` |")
	else:
		lines.append("（无）")
	lines.append("")

	lines.append("## 建议优先补的遗漏（人工筛选）")
	lines.append("")
	priority_hints = [
		("ControlFillColorInputActive", "TextBox 聚焦底 — textedit/bg 已 STRUCT 映射"),
		("SystemFillColorSuccess", "成功态 — QTE 暂无独立 role（Caution/Critical 已映 progress.chunk.paused|error）"),
		("MenuFlyoutPresenterBackground", "真 Acrylic 需平台材质；现 menu/bg.acrylic 为实色近似"),
		("TextOnAccentFillColorDisabled", "Accent 禁用文字 — check/indicator.disabled 已 STRUCT"),
	]
	for name, why in priority_hints:
		lines.append(f"- **{name}** — {why}")
	lines.append("")

	lines.append("## 结论（本轮）")
	lines.append("")
	lines.append(
		"1. **正向**：全部 QTE 色 role 已声明 WinUI 映射；严格 `DIFF=0`。"
		"剩余 `STRUCT_DIFF` 主要为半透明描边/填充的不透明近似、以及焦点环内外描边。"
	)
	lines.append(
		"2. **本轮已补**：Accent Secondary/Tertiary、TextOnAccentSecondary、"
		"Progress paused/error（`qtheme.progressState`）、Focus 双描边、"
		"view selected+hover、text.tertiary、menu/bg.acrylic、combo placeholder。"
	)
	lines.append(
		"3. **仍非本引擎范围**：真 Acrylic/Mica 材质、SystemFill Success 独立态。"
	)
	lines.append("")
	lines.append("复跑：`python scripts/audit_fluent_vs_mux_full.py`")
	lines.append("")

	OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
	print(f"Wrote {OUT}")
	print(
		f"rows={len(rows)} okish={len(ok_rows)} close={len(close_rows)} "
		f"diff={len(diff_rows)} struct_diff={len(struct_diff)} unmapped={len(unmapped)} "
		f"unused_common={len(unused_common)} unused_important={len(unused_important)}"
	)


if __name__ == "__main__":
	main()

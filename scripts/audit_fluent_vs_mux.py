#!/usr/bin/env python3
"""Compare QThemeEngine Fluent packs against microsoft-ui-xaml Common theme colors."""
from __future__ import annotations

import json
import re
from pathlib import Path

MUX = Path(r"D:\Codes\microsoft-ui-xaml\controls\dev\CommonStyles\Common_themeresources_any.xaml")
TEXTBOX = Path(r"D:\Codes\microsoft-ui-xaml\controls\dev\CommonStyles\TextBox_themeresources.xaml")
QTE_DARK = Path(r"D:\Codes\QThemeEngine\resources\themes\fluent\fluent.dark.theme.json")
QTE_LIGHT = Path(r"D:\Codes\QThemeEngine\resources\themes\fluent\fluent.light.theme.json")


def parse_theme_colors(xaml: str, block_key: str) -> dict[str, str]:
	m = re.search(
		rf'<ResourceDictionary x:Key="{block_key}">(.*?)</ResourceDictionary>',
		xaml,
		re.S,
	)
	if not m:
		return {}
	return {
		cm.group(1): cm.group(2).upper()
		for cm in re.finditer(r'<Color x:Key="([^"]+)">#([0-9A-Fa-f]+)</Color>', m.group(1))
	}


def parse_argb(h: str) -> tuple[int, int, int, int]:
	h = h.upper()
	if len(h) == 6:
		return 255, int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16)
	if len(h) == 8:
		return int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16), int(h[6:8], 16)
	raise ValueError(h)


def composite(fg_hex: str, bg_hex: str) -> str:
	fa, fr, fg, fb = parse_argb(fg_hex)
	_, br, bg, bb = parse_argb(bg_hex if len(bg_hex) != 6 else "FF" + bg_hex)
	a = fa / 255.0
	r = round(fr * a + br * (1 - a))
	g = round(fg * a + bg * (1 - a))
	b = round(fb * a + bb * (1 - a))
	return f"#{r:02X}{g:02X}{b:02X}"


def to_opaque(mux_hex: str, bg: str | None) -> str:
	if len(mux_hex) == 6:
		return "#" + mux_hex
	if len(mux_hex) == 8 and mux_hex.startswith("FF"):
		return "#" + mux_hex[2:]
	return composite(mux_hex, bg or "000000")


def load_qte(path: Path) -> dict[str, str]:
	data = json.loads(path.read_text(encoding="utf-8"))
	out: dict[str, str] = {}
	for group, roles in data.get("colors", {}).items():
		for role, val in roles.items():
			out[f"{group}/{role}"] = str(val).upper()
	return out


def rgb(h: str) -> tuple[int, int, int]:
	h = h.lstrip("#")
	if len(h) == 8:
		h = h[2:]
	return tuple(int(h[i : i + 2], 16) for i in (0, 2, 4))  # type: ignore[return-value]


def main() -> None:
	xaml = MUX.read_text(encoding="utf-8")
	dark_mux = parse_theme_colors(xaml, "Default")
	light_mux = parse_theme_colors(xaml, "Light")
	qte_dark = load_qte(QTE_DARK)
	qte_light = load_qte(QTE_LIGHT)

	dark_surface = dark_mux["SolidBackgroundFillColorQuarternary"]  # 2C2C2C
	dark_base = dark_mux["SolidBackgroundFillColorBase"]  # 202020
	light_bg = "FFFFFF"

	# Confirm TextBox mapping
	tb = TEXTBOX.read_text(encoding="utf-8")
	assert "TextControlPlaceholderForeground" in tb and "TextFillColorSecondaryBrush" in tb

	print("Source:", MUX)
	print("TextBox placeholder maps to TextFillColorSecondaryBrush (confirmed in TextBox_themeresources.xaml)")
	print()
	print("WinUI Dark Secondary raw", dark_mux["TextFillColorSecondary"], "->", to_opaque(dark_mux["TextFillColorSecondary"], dark_surface))
	print("WinUI Dark Tertiary  raw", dark_mux["TextFillColorTertiary"], "->", to_opaque(dark_mux["TextFillColorTertiary"], dark_surface))
	print("WinUI Light Secondary raw", light_mux["TextFillColorSecondary"], "->", to_opaque(light_mux["TextFillColorSecondary"], light_bg))
	print()

	checks = [
		("dark", "palette/window", "SolidBackgroundFillColorBase", None, "window"),
		("dark", "palette/base", "SolidBackgroundFillColorQuarternary", None, "surface/input"),
		("dark", "palette/text", "TextFillColorPrimary", None, "primary text"),
		("dark", "edit/fg", "TextFillColorPrimary", None, "edit text"),
		("dark", "edit/fg.placeholder", "TextFillColorSecondary", dark_surface, "placeholder MUST be Secondary"),
		("dark", "textedit/fg.placeholder", "TextFillColorSecondary", dark_surface, "placeholder MUST be Secondary"),
		("dark", "edit/fg.disabled", "TextFillColorDisabled", dark_surface, "disabled"),
		("dark", "button/fg", "TextFillColorPrimary", None, "button text"),
		("dark", "button/fg.disabled", "TextFillColorDisabled", dark_surface, "button disabled"),
		("dark", "button/bg", "ControlSolidFillColorDefault", None, "opaque approx of translucent ControlFill"),
		("dark", "menu/bg", "SolidBackgroundFillColorTertiary", None, "flyout-ish surface"),
		("dark", "tooltip/bg", "SolidBackgroundFillColorTertiary", None, "tooltip often acrylic; solid approx"),
		("light", "palette/window", "SolidBackgroundFillColorBase", None, "window"),
		("light", "edit/fg", "TextFillColorPrimary", light_bg, "edit text"),
		("light", "edit/fg.placeholder", "TextFillColorSecondary", light_bg, "placeholder MUST be Secondary"),
		("light", "textedit/fg.placeholder", "TextFillColorSecondary", light_bg, "placeholder MUST be Secondary"),
		("light", "edit/fg.disabled", "TextFillColorDisabled", light_bg, "disabled"),
		("light", "button/fg", "TextFillColorPrimary", light_bg, "button text"),
	]

	print(f"{'theme':<6} {'qte':<28} {'got':<10} {'expect':<10} {'flag':<6} note")
	mismatches = []
	for theme, qkey, mkey, bg, note in checks:
		muxc = dark_mux if theme == "dark" else light_mux
		qte = qte_dark if theme == "dark" else qte_light
		exp = to_opaque(muxc[mkey], bg)
		got = qte.get(qkey, "MISSING")
		if got == "MISSING":
			delta = 999
			flag = "MISS"
		else:
			gr, gg, gb = rgb(got)
			er, eg, eb = rgb(exp)
			delta = max(abs(gr - er), abs(gg - eg), abs(gb - eb))
			flag = "OK" if delta <= 8 else ("CLOSE" if delta <= 24 else "DIFF")
		print(f"{theme:<6} {qkey:<28} {got:<10} {exp:<10} {flag:<6} {note}")
		if flag in {"DIFF", "MISS"}:
			mismatches.append((theme, qkey, got, exp, note))

	print()
	print("Summary mismatches (DIFF/MISS):", len(mismatches))
	for row in mismatches:
		print(" -", row)

	print()
	print("Structural notes (not pixel-equal by design):")
	print(" - WinUI ControlFill* / SubtleFill* are translucent overlays; QTE uses opaque solids.")
	print(" - AccentFillColorDefault = SystemAccentColorLight2 (OS accent); QTE samples #60CDFF / #0078D4.")
	print(" - ToolTip/Menu often Acrylic; QTE uses solid surfaces.")


if __name__ == "__main__":
	main()

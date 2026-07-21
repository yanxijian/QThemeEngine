# QThemeEngine Architecture

> **中文主文档**：[../zh/architecture.md](../zh/architecture.md)

## Goal

Replace **Qt Style Sheets (QSS)** for Qt Widgets with a **theme-table-driven custom `QStyle`** (`QThemeStyle`).

- No QSS as the theming channel.  
- **ThemeStore** holds colors / fonts / metrics (roles).  
- **QThemeStyle** paints and sizes built-in widgets by querying the store.  
- Owner-drawn widgets may use the same store (secondary path).

## Core pipeline

```text
Theme pack → ThemeStore → QThemeStyle → QApplication::setStyle
                         → ThemeApi (owner-draw)
                         → QPalette
```

## Milestones

| ID | Focus |
|----|--------|
| M0 | Store seed + Engine::apply + QThemeStyle skeleton; native `QPushButton` recolors without QSS |
| M1 | Load `.theme.xml` + light/dark |
| M2+ | Expand [coverage matrix](../zh/coverage-matrix.md) |

Details, principles, Theme Pack / Accent / HC: see the Chinese architecture doc (§4.3–4.6).
Widget catalog for planning: [qt-widgets-inventory.md](qt-widgets-inventory.md) (Chinese canonical).

# QThemeEngine development plan

<!-- translated-from: docs/zh/dev-plan.md sha256:fd18ad8a6f07 -->

> **中文主文档**：[../zh/dev-plan.md](../zh/dev-plan.md)  
> Status: near/mid-term investment notes — does **not** replace [architecture.md](architecture.md).  
> Updated: 2026-10-04 (deferred items verified against tree)

---

## Stance

| Conclusion | Note |
|------------|------|
| **No whole-repo rewrite** | Product seam is sound: Pack → ThemeStore → QThemeStyle / `api`, orchestrated by `Engine` |
| **Delivery first** | Fluent coverage, WinUI (MUX) alignment, focus/control polish — see coverage matrix |
| **Deepen when touching** | Incremental deepening only; no standalone “refactor season” |

M0–M6 are done. No new M7 id. Non-goals remain in architecture §9.

## Deepening candidates

| Strength | Candidate | Status |
|----------|-----------|--------|
| **Strong** | Carve `QThemeStyle` **internals** by control family | **Paint path done** (`src/qtheme/style/*`); public `QStyle` seam unchanged. **Metrics / subElement still in** `style.cpp` |
| Worth exploring | Peel Format out of ThemeStore | Deferred |

Internals: `StyleCtx` + paint util; family files for edit / check / focus / button / menu / itemview / tab_header / progress / chrome / complex; `style.cpp` dispatches + polish / palette / metrics.

## Deferred (verified, later)

| Item | Fact | Later |
|------|------|-------|
| Shared painter for owner-draw | `qtheme::api` reads the store; `DemoButton` still hand-paints | Thin `ThemePainter` (fill / stroke / label) so third-party widgets do not copy DemoButton |
| Hot-path lookup | **Done (2026-10-04)**: `ColorLookupCache` on `QThemeStyle`, invalidated on store generation | Optional: enum-indexed / `QBrush` cache |

Pack JSON recursion is materialize-time, not per `drawControl`.


## Explicitly not now

- Pre-refactoring app-shell / host layers for speculative product integrations  
- Inventing a second Format adapter (including restoring `.theme.xml`)  
- Splitting `Engine` into multiple public façades without Interface pressure

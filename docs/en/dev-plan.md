# QThemeEngine development plan

> **中文主文档**：[../zh/dev-plan.md](../zh/dev-plan.md)  
> Status: near/mid-term investment notes — does **not** replace [architecture.md](architecture.md).  
> Updated: 2026-07-25 (Style paint-path family split landed)

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
| Speculative | Quarantine / drop `theme::` stubs | Deferred |

Internals: `StyleCtx` + paint util; family files for edit / check / focus / button / menu / itemview / tab_header / progress / chrome / complex; `style.cpp` dispatches + polish / palette / metrics.

## Explicitly not now

- Pre-refactoring for a future MultiProcessShell Host integration  
- Inventing a second Format adapter “for the seam”  
- Splitting `Engine` into multiple public façades without Interface pressure

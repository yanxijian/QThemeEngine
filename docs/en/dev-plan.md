# QThemeEngine development plan

> **中文主文档**：[../zh/dev-plan.md](../zh/dev-plan.md)  
> Status: near/mid-term investment notes — does **not** replace [architecture.md](architecture.md).  
> Updated: 2026-07-25 (`codebase-design` + `improve-codebase-architecture`; discuss later)

---

## Stance

| Conclusion | Note |
|------------|------|
| **No whole-repo rewrite** | Product seam is sound: Pack → ThemeStore → QThemeStyle / `api`, orchestrated by `Engine` |
| **Delivery first** | Fluent coverage, WinUI (MUX) alignment, focus/control polish — see coverage matrix |
| **Deepen when touching** | Incremental deepening only; no standalone “refactor season” |

M0–M6 are done. No new M7 id. Non-goals remain in architecture §9.

## Deepening candidates (recorded, deferred)

| Strength | Candidate |
|----------|-----------|
| **Strong** | Carve `QThemeStyle` **internals** by control family (external `QStyle` seam unchanged) |
| Worth exploring | Peel Format out of ThemeStore (`PackRegistry` ownership) |
| Speculative | Quarantine / drop `theme::` stubs until a second Format adapter exists |

If the next weeks are Fluent-only, prefer the Strong Style carve — matches commit hot spots.

## Explicitly not now

- Pre-refactoring for a future MultiProcessShell Host integration  
- Inventing a second Format adapter “for the seam”  
- Splitting `Engine` into multiple public façades without Interface pressure

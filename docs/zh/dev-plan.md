# QThemeEngine 开发计划

> **English**：[../en/dev-plan.md](../en/dev-plan.md)  
> **地位**：近中期投入与架构姿态的备忘；**不**替代 [architecture.md](architecture.md)。  
> **更新**：2026-07-25（`codebase-design` + `improve-codebase-architecture` 评审结论入库；细节待下次讨论）

---

## 1. 当前姿态

| 结论 | 说明 |
|------|------|
| **不做整仓重构** | 产品缝已清晰：Pack → ThemeStore → QThemeStyle / `api`，由 `Engine` 编排 |
| **主线仍是交付** | Fluent 覆盖、对照 WinUI（MUX）、focus / 控件打磨；见 [coverage-matrix.md](coverage-matrix.md) |
| **加深随改动发生** | 碰到相关文件再 deepening，不为「干净」单独开重构季 |

里程碑 M0–M6 已落地；无新的 M7 编号。架构非目标见 [architecture.md §9](architecture.md)。

---

## 2. 加深候选（已记录，待择机讨论）

强度来自 2026-07-25 架构扫描（热点：`src/qtheme/style.cpp`、Fluent packs）。

| 强度 | 候选 | 要点 |
|------|------|------|
| **Strong** | 按控件族拆分 `QThemeStyle` **内部**实现 | 外部仍是一个 `QStyle` seam；改善 locality / AI 导航；不换公开 Interface |
| Worth exploring | Format 从 ThemeStore 剥离 | `loadBuiltinPack` 等不再让 Store 构造 `PackRegistry` |
| Speculative | 隔离或默认构建去掉 `theme::` stub | ThemeLoader / SkinManager / ThemeApi 删除测试几乎不搬产品复杂度；第二 Format adapter 出现前 seam 仍是假想 |

**近中期若只做 Fluent**：优先考虑 Strong 项（Style 内拆），与热点一致。

---

## 3. 明确暂不做

- 为「将来接 MultiProcessShell」预重构 Host 集成层  
- 空抽第二主题 Format adapter（one adapter = hypothetical seam）  
- 把 Engine 拆成多个公开门面（除非 Interface 继续明显膨胀）

---

## 4. 相关文档

| 文档 | 角色 |
|------|------|
| [architecture.md](architecture.md) | 产品主路径（权威） |
| [coverage-matrix.md](coverage-matrix.md) | 控件覆盖进度 |
| [fluent-mux-audit.md](fluent-mux-audit.md) | Fluent ↔ WinUI 色审计 |

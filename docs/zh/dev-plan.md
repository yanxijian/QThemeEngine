# QThemeEngine 开发计划

> **English**：[../en/dev-plan.md](../en/dev-plan.md)  
> **地位**：近中期投入与架构姿态的备忘；**不**替代 [architecture.md](architecture.md)。  
> **更新**：2026-07-25（Style 绘制路径按控件族加深落地）

---

## 1. 当前姿态

| 结论 | 说明 |
|------|------|
| **不做整仓重构** | 产品缝已清晰：Pack → ThemeStore → QThemeStyle / `api`，由 `Engine` 编排 |
| **主线仍是交付** | Fluent 覆盖、对照 WinUI（MUX）、focus / 控件打磨；见 [coverage-matrix.md](coverage-matrix.md) |
| **加深随改动发生** | 碰到相关文件再 deepening，不为「干净」单独开重构季 |

里程碑 M0–M6 已落地；无新的 M7 编号。架构非目标见 [architecture.md §9](architecture.md)。

---

## 2. 加深候选

| 强度 | 候选 | 状态 |
|------|------|------|
| **Strong** | 按控件族拆分 `QThemeStyle` **内部**实现 | **绘制路径已加深**（`src/qtheme/style/*`）；公开 `QStyle` seam 不变。**度量 / subElement 仍在** `style.cpp`，待后续按需再拆 |
| Worth exploring | Format 从 ThemeStore 剥离 | 待讨论 |

内部布局备忘：`StyleCtx` + `style_paint_util`；族文件含 edit / check / focus / button / menu / itemview / tab_header / progress / chrome / complex；`style.cpp` 负责分发与 polish / palette / metrics。

---

## 3. 明确暂不做

- 为空想产品集成预重构应用壳 / 宿主层  
- 空抽第二主题 Format adapter（含恢复 `.theme.xml`）  
- 把 Engine 拆成多个公开门面（除非 Interface 继续明显膨胀）

---

## 4. 相关文档

| 文档 | 角色 |
|------|------|
| [architecture.md](architecture.md) | 产品主路径（权威） |
| [coverage-matrix.md](coverage-matrix.md) | 控件覆盖进度 |
| [fluent-mux-audit.md](fluent-mux-audit.md) | Fluent ↔ WinUI 色审计 |

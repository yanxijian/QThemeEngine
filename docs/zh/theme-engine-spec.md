# 主题格式说明（历史归档）

> **English index**：[../en/README.md](../en/README.md)  
> **产品主路径（必读）**：[architecture.md](architecture.md) · [coverage-matrix.md](coverage-matrix.md)

## 状态

早期 L0 设计曾规划 `.theme.xml` Format（`theme::ThemeLoader` / `SkinManager`）。**该路径从未作为产品能力交付**，相关 stub 代码与样例资源已从仓库移除。

**当前唯一主题数据格式**：JSON Theme Pack（`*.theme.json`），经 `qtheme::PackRegistry` / `qtheme::Engine` 加载。契约与 Token 约定见 [architecture.md §4](architecture.md)。

请勿再新增 XML 主题加载器或双格式运行时；若需迁移旧皮，做离线转换进 JSON，而不是恢复 XML 通道。

# CI（GitHub Actions）

> **English**：[../en/ci.md](../en/ci.md)

工作流：[`.github/workflows/ci.yml`](../../.github/workflows/ci.yml)

| Job | Runner | 内容 |
|-----|--------|------|
| `clang-format` | ubuntu | `scripts/format_source.py --check` |
| `Windows MSVC + Qt` | windows-latest | Qt 6.8.3 + 构建 / `ctest` |
| `Ubuntu + Qt` | ubuntu-latest | Qt 6.8.3 + `QT_QPA_PLATFORM=offscreen` 跑测 |

触发：`push`/`pull_request`，以及 `workflow_dispatch`。  
Dependabot：[`.github/dependabot.yml`](../../.github/dependabot.yml)。

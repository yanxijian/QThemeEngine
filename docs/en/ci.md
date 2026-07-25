# CI (GitHub Actions)

> **中文**：[../zh/ci.md](../zh/ci.md)

Workflow: [`.github/workflows/ci.yml`](../../.github/workflows/ci.yml)

| Job | Runner | What |
|-----|--------|------|
| `clang-format` | ubuntu | clang-format 20 (PyPI) + `scripts/format_source.py --check` |
| `Windows MSVC + Qt` | windows-latest | Qt 6.8.3 + build / `ctest` |
| `Ubuntu + Qt` | ubuntu-latest | Qt 6.8.3 + offscreen `ctest` |

Triggers: push/PR and `workflow_dispatch`. Dependabot updates Actions weekly.

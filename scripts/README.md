# scripts/

| Script | Purpose |
|--------|---------|
| `format_source.py` | Format C++ with root `.clang-format` (skips `build/`) |

CI: [docs/zh/ci.md](../docs/zh/ci.md) / [docs/en/ci.md](../docs/en/ci.md).

```bat
python scripts\format_source.py
python scripts\format_source.py --check
```

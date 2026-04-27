# ATEParser Optimization Report (2026-04-28)

## Scope
This report summarizes implementation and measurement results for three parser optimizations:

1. Conditional `raw_data` copy (`keep_raw` path)
2. `@RPT` literal length parser (`std::stoi` -> `std::from_chars`)
3. `parse_bool` allocation removal (no lowercase temporary string)

## Validation Status
- Unit tests: **PASSED**
- Result: **74/74 test cases passed**
- Command used:

```powershell
Set-Location d:\workspace\!PSZ\!ATE\!ATE_workspace4\ATEParser
cmake --build build --target bench_parse unit_tests -j 6
.\build\tests\unit_tests.exe
```

## Benchmark Setup
- Benchmark tool: `bench_parse`
- Sample file:
  - `..\.example_logs\_processed\P1086592-22-L_AAAA00000000001_260323035023_MY59250383_Pass.txt`
- File size: `108,516 bytes`
- Iterations: `400`
- Improvement rate formula:

$$
\text{Improvement Rate (\%)} = \frac{\text{Before} - \text{After}}{\text{Before}} \times 100
$$

## End-to-End Parse Pipeline (context)
| Metric | Value |
|---|---:|
| Parse only (total) | 866.134 ms |
| Parse only (per iter) | 2.16534 ms |
| Parse + JSON (total) | 2700.21 ms |
| Parse + JSON (per iter) | 6.75053 ms |
| JSON delta (per iter) | 4.58519 ms |

## Improvement Results (3 items)
| Improvement | Before (ms) | After (ms) | Improvement Rate |
|---|---:|---:|---:|
| `keep_raw` conditional copy | 660.215 | 540.805 | **18.0865%** |
| `@RPT` literal length parse (`stoi` -> `from_chars`) | 480.907 | 48.39 | **89.9378%** |
| `parse_bool` lower-copy removal | 133.48 | 89.196 | **33.1765%** |

## Notes
- The three rates above are measured directly by `bench_parse` with before/after code paths in the same executable.
- Absolute numbers can vary by CPU scheduling and machine load; relative trends are stable.
- `@RPT` parsing change shows the largest micro-level gain due to removal of temporary string allocation and exception-based parsing overhead.

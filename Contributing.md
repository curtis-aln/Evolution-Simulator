# Contributing to Project A.R.I.A

Thanks for your interest in contributing to this project ARIA. It's currently a solo project at an early/beta stage, so contributions are welcome but the workflow is kept light.

## Getting Started

1. Fork the repo and clone your fork.
2. Follow the build steps in the [README](README.md) (CMake 3.21+, C++23 compiler, SFML 3 fetched automatically via CMake).
3. Confirm the project builds and runs (`ProjectARIA`) before making changes.

## Ways to Contribute

Areas currently most useful, per the README's Known Limitations:

- **Stats graphs** — rendering breaks at non-standard resolutions
- **Save/load** — no simulation state persistence yet
- Bug reports, small optimizations, and doc/comment improvements are always welcome

## Code Style

- Match the existing structure: header + `.cpp` split (see `src/Protozoa/`, `src/world/`), extension logic under `extension/`
- Simulation-wide constants belong in `settings.h`, not scattered magic numbers
- Follow existing naming conventions in the file/class you're editing (the codebase is consistent internally, if not against a single universal standard)
- Keep hot-path code (physics, spatial grid, rendering) allocation-free where the surrounding code already is

## Submitting Changes

1. Create a feature branch off `master` (`git checkout -b fix/short-description`)
2. Keep PRs small and focused on one change
3. Describe what you tested — there's no automated test suite, so manual verification (what you ran, what you observed) matters
4. Include before/after screenshots or a short clip for anything visual (rendering, UI, debug overlay)
5. Open the PR against `master` with a clear description of the change and why

## Reporting Bugs / Crashes

- If the sim crashed, attach the relevant portion of `crash.log` (written automatically via `CrashLogger`)
- Include your OS, compiler, and build config (Debug/Release)
- Steps to reproduce, or the settings/`aria_settings.toml` values in use, help a lot

## Questions

Open an issue — no formal template required, just enough context to act on.
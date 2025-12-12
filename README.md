# Project A.R.I.A

Project A.R.I.A is a realtime 2D organic simulation and sandbox built with C++ and SFML. It simulates protozoa-like organisms composed of cells and springs, evolving over time through a simple genetic system. The project is intended for experimentation with emergent behaviour, procedural genetics, and realtime physics.

---

## Highlights
- Real-time physics-driven organisms composed of cells and springs
- Per-object genes (sin-wave parameters) and a genome system that mutates over generations
- Spatial hashing for large numbers of food particles
- Thread-pool for background tasks and simulation parallelism
- Editor / builder tools and runtime debug overlays


## Screenshots
![Overview](Project-Aria/media/readme_images/Evolution-Run-timestep-10,000.png)

![In-Editor](media/readme_images/Evolution-Run-timestep-40,000.png)

![Debug View](media/readme_images/Evolution-Run-timestep-100,000.png)


## Getting started
### Prerequisites
- Windows or Linux development environment (Visual Studio recommended on Windows)
- C++17/20 compiler (the project is developed with modern C++ features)
- SFML (tested with 2.6) — headers and libraries available on PATH or configured in the project
- CMake / Visual Studio solution (the repository includes a Visual Studio project)

### Build (Windows / Visual Studio)
1. Open `Project ARIA.sln` or the Visual Studio project in the `Project ARIA` folder.
2. Make sure SFML include/lib directories are set in project properties or installed system-wide.
3. Build the solution in Release or Debug.
4. Copy the `media` folder next to the executable (or configure working directory in the debugger).

### Run
- From Visual Studio set working directory to the repository root or the `bin` folder containing the `media` directory so the engine can find `media/fonts` and `media/readme_images`.
- Run the built executable. Use the in-app UI to spawn protozoa, toggle debug overlays, or open the builder.


## Controls (default)
- Left click: select
- Left hold: drag
- Right hold: add link (builder mode)
- Use on-screen UI buttons for simulation controls and presets


## Architecture notes
- `Protozoa` objects contain `Cell` and `Spring` objects; `Genome` centralizes mutation settings and per-object genes.
- `o_vector` is a custom fast object pool used in various managers (optimized for reuse).
- `spatial_hash_grid` is used to accelerate proximity queries for food and interactions.
- `ThreadPool` provides a small worker thread pool with a task queue.


## Development tips & known bottlenecks
- Rendering many individual SFML draw calls is expensive; consider batching via `sf::VertexArray` where possible.
- Avoid caching raw pointers into `std::unordered_map` values — use contiguous storage (vectors) or stable smart pointers.
- Use the provided `Genome` APIs: `init_genome_dictionaries`, `mutate_genome`, and `get_cell_gene/get_spring_gene`.


## Contributing
- Fork the repo and open a feature branch.
- Keep changes small and focused; add unit tests where applicable.
- Run the build and verify there are no warnings before opening a PR.


## License
Specify the license used by your project here (MIT, Apache-2.0, GPL, etc.).


---

If you want, I can:
- Replace the placeholder image names with the exact filenames present in `media/readme_images`.
- Add a short GIF of simulation playback (if you provide one).
- Generate a `CONTRIBUTING.md` and `CODE_OF_CONDUCT.md`.


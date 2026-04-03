// NOTE: do not use cell id's as indexes as when cells are added and removed it can mess up
// NOTE: if there are multithreading issues check how rendering works with the update thread because i commented out the lock mutex

// ********** Currently Working On ********** //


/* Graphics */
// Create a RectBuffer and re-introduce the connections
// Cells should fade in and out of existance
// Statistics graphs should be constantly at the bottom right corner of the screen
// Food and cell counts should be on the same graph
// sort out recaling issues with the line graph
// add statistics statically on the screen aswell
// make a "draw_static" function for the camera


/* Debug */
// cell phase and friction dp need to be trimmed
// cell phase and friction spacing too small
// show cell generation
// move all cell information to top corner of bounding box
// - track average mutation rate / mutation range accross all protozoa
// - display offspring count and track average accross all protozoa
// - track average cells accross all protozoa

// add o_vector compatability for the circleBuffer and spatial_hash_grid_


/* To-fix before releasing public version */
// - remove_cell() is a stub - implement: remove cell from m_cells_, remove connected springs from m_springs_, call remove_cell_gene() and remove_spring_gene() for each. Ensure IDs remain valid (consider ID remapping or tombstoning).

// - Implement spring connection creation
// - nearby_ids shared array — correctness bug & data race - Change to a local variable inside update_grid_cell() or use a per-thread buffer. Remove the #pragma omp stubs until threading is intentionally added. 
// - Death + reproduce same frame = invalid at() call - Process reproduction only after fully completing the death/remove pass. Use a separate reproduce_indexes collected before any removals and validate the index is still active before calling create_offspring.
// [DONE] - Add LICENSE file - Choose MIT or similar, add LICENSE to repo root, update README.
// - Unbounded gene drift causing mass spring-breaking deaths - Clamp amplitude, frequency, and vertical_shift after mutation. Use std::numbers::pi instead of the literal 3.14159f. Decide on biologically sensible gene ranges.
// - FoodManager UB — dereferencing pointer before assignment - Move bounds_radius and world_rect_bounds into the constructor body, or pass them as constructor parameters before member init.
// [DONE] No binary release for beta testers - Build a Release binary on Windows (and ideally Linux). Attach to a GitHub Release with a zip containing the exe + media/ folder.
// [DONE] get_average_generation() divide-by-zero - Guard with if (all_protozoa_.size() == 0) return 0.f;
// - Builder stubs (builder_add_cell, make_connection) - Either implement them properly, or hide the builder UI entirely until they work. Don't ship broken UI features to beta testers.
// - Thread pool created but never used (8 idle threads) - Either wire dispatch() into update_all_protozoa() and the collision grid loop, or reduce to 0 threads and remove it until you're ready to parallelize properly.
// [DONE] CMake project named MyApp - Rename project(MyApp) and add_executable(MyApp) to ProjectARIA or similar.
// - file(GLOB_RECURSE) in CMake - Replace with an explicit list of .cpp files, or add a comment warning to re-run CMake when files are added.
// [DONE] - #include ordering in settings.h - Move #include <unordered_map> and #include <vector> to the top of the file.
// - Food eating uses AABB not per-cell collision - Replace m_personal_bounds_.contains() with a per-cell circle check: iterate m_cells_, check dist_sq(cell.pos, food.pos) < (cell.radius + food_radius)^2.
// - std::cout debug prints in production code - Add a constexpr bool DEBUG_LOGGING = false flag in settings and gate all std::cout behind it, or use a proper logger.
// [DONE] No crash handling — exceptions kill silently - Wrap Simulation::run_simulation() in main() with a try/catch that writes to a crash.log file and shows an sf::Text error screen before closing.
// - double texcoords written twice in CircleBatchRenderer - Delete the duplicate texcoord block (lines u0,v0 through u0,v1 are copy-pasted twice).
// [DONE] README references .sln file that doesn't exist - Update README to only document the CMake build path, remove the Visual Studio .sln reference. Add clear step-by-step CMake build instructions.
// - min_speed rising floor poorly documented - Add a comment or UI label explaining it as a selective pressure mechanic. Expose it as a tunable in settings. Consider whether it should apply to all protozoa equally.
// - o_vector::add() is O(N) - Add a free-list (stack of inactive indices) to make add/remove O(1). This matters at 30k organisms.
// - Simulation inherits from settings structs - Refactor settings to be accessed as static structs directly (they already are static), and remove the private inheritance from Simulation, World, etc.
// [DONE] Reproduction threshold too low (2 food items) - Balance test: raise threshold, add energy cost to reproduction, consider a size-scaled energy requirement.
// [DONE] All keyboard controls not in README - Add a full controls table to the README.
// [DONE] rand_pos_in_circle infinite loop risk - Add a maximum iteration count with a fallback (return center if exceeded) or use the polar coordinate method instead.
// [DONE] add todo, crashlog to the gitignore

// [DONE] text doesnt show when you run the executable
// - make the spatial hash grid scalable, then have the world gradually increase in size
// - implement pezzas work datastructure 
// [DONE] translating the screen should have accelaration & de-accelaration instead of being instant
// - you cant select protozoa without holding 
// [DONE] statistics continue even when simulation is paused, should pause them aswell
// [DONE] textbox showing stats and controls is messed up
// each protozoa stores cell_positions_nearby and food positions for debugging, just have it once in the protozoa manager

ImGUI Todo
[Done] add x and y axis indicators to Line graph
[Done] min speed and delta min speed are redundant
[Done] statistic for time per generation change
[Done] statistic for average amount of offspring
[Done] statistic for average lifetime
[Done] statistic for average mutation rate and mutation range
[Done] statistic for average number of cells per protozoa
[Done] make the buttons actuall buttons instead of checkboxes and add the keybind next to it, then remove it from the controls
[Done] Align the buttons better
[Done] pressing o when not paused should pause the sim
[Done] When you click on an organism a statistics page should show up showing all the information about that organism
[Done] the x and y space it takes up plus its square area
[Done] distance to its birth location
[Done] Move the debug control settings and buttons into this mini popup menu
[Done] Ability to hide IMGUI screens completely with a keybind (for screenshots)
- Reset Simulation Button with controls for world size, initial protozoa count, food spawn rate, and mutation rate/range
		- add all settings to a json file and have the program read from it on starrtup, then add a "save settings" button to write the current settings back to the file. 
This allows for easy tweaking without recompiling, and sharing of settings between users.

- Simulation should start more zoomed out
- add protozoa and food population counts to the statistics graph instead 
[Done] paused statistic on the main bit is redundant
- add the ability to change the worldsize in real time, regardless of if the spatial hash grid can change with it yet
- add the ability to pinch, pin, and throw organisms around 
- add a zoom slider for users who are using a trackpad
- add a "Navigate to most sucessfull organism" button which locks on to the organism which has reporoduced the most
- add the ability to save organisms to file
- add the ability to spawn a saved organism
- add the ability to fill the world with a saved organism (with mutations optional)
- Add a "total" for the protozoa, food line graph
- Add a layered background like you saw in that video
- Add ambient music with mild bubble sound effects
- Add black holes which can pull in protozoa and food, but not kill them, just make them orbit until they get pulled out by another protozoa or food item
  can be modified by the user. if the gravitational pull is negative it becomes a white hole which pushes things away instead of pulling them in
- Worlds should be able to be saved and loaded
- Add radiation zones which mutates an organism based on their proximity to the center of the zone, the closer they are the higher the mutation rate and range, but also the higher the energy cost for existing in that zone
- Organism tagging, ability to tag an organism and it will be outlined and visible, will show up on the tagged organisms screen
- Add tabs for these IMgui screens because i imagine there will be a lot of them eventually and it will get cluttered, so maybe have one for statistics, one for controls, one for the tagged organisms, etc.
- Time-lapse / fast-forward mode — run simulation at 10x-100x with minimal rendering for long-run experiments
- Ability to make organisms immortal
- Ability to force organism to reproduce or die
- Force feed: manually inject energy into a selected organism, useful for keeping a favourite alive during experiments
- Genome editor: directly tweak a selected organism's trait values via sliders, bypassing mutation. Great for controlled experiments
- Clone: duplicate an organism exactly, spawn the copy nearby
- Death notifications: optional toast notification when a tagged organism dies, with cause of death (starvation, age, etc.
- statistic showing total alive entities
- and then last but not least, figure out how to put rendering and updating on seperate threads, then you can publish a video
- create a cell body class
- spatial hash grid should dynamically change density as the population changes, and should be able to change size too
- when a new cell is created it should have very low friction and generally not affect the organism too much, test this
- add infant mortality stat

Update Name: Playing God

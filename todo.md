----------------------------------------------------------------------
##### ImGUI TODO
[Done] time elapsed in hours minutes seconds
- add infant mortality stat
- Death notifications: optional toast notification when a tagged organism dies, with cause of death (starvation, age, etc.
- Clone: duplicate an organism exactly, spawn the copy nearby
- Organism tagging, ability to tag an organism and it will be outlined and visible, will show up on the tagged organisms screen
- Time-lapse / fast-forward mode — run simulation at 10x-100x with minimal rendering for long-run experiments
- Ability to make organisms immortal
- Force feed: manually inject energy into a selected organism, useful for keeping a favourite alive during experiments
- Worlds should be able to be saved and loaded
- add the ability to change the worldsize in real time, regardless of if the spatial hash grid can change with it yet
- add the ability to pinch, pin, and throw organisms around 
- add a zoom slider for users who are using a trackpad
- add a "Navigate to most sucessfull organism" button which locks on to the organism which has reporoduced the most
- add the ability to save organisms to file
- add the ability to spawn a saved organism
- add the ability to fill the world with a saved organism (with mutations optional)
- Add a "total" for the protozoa, food line graph
- Reset Simulation Button with controls for world size, initial protozoa count, food spawn rate, and mutation rate/range
- add all settings to a json file and have the program read from it on starrtup, then add a "save settings" button to write the current settings back to the file. 
- line graph for average lifetime
- line graph for mutation rate and range
- line graph for average offspring
- Track collision resolutions per frame
- Get remove, add spring, and remove spring working
- get stomach bar working correctly
- get time since last reproduced working better, replace with reproduce cooldown
- when cells are immortal their energy shouldnt drop below zero
- sometimes the sin wave graph goers by too quick, slider to change the amount of cycles displayed

-----------------------------------------------------------------------

##### world TODO
- spatial hash grid should dynamically change density as the population changes, and should be able to change size too
- Add radiation zones which mutates an organism based on their proximity to the center of the zone, the closer they are the higher the mutation rate and range, but also the higher the energy cost for existing in that zone
- Add black holes which can pull in protozoa and food, but not kill them, just make them orbit until they get pulled out by another protozoa or food item
  can be modified by the user. if the gravitational pull is negative it becomes a white hole which pushes things away instead of pulling them in
- Add a layered background like you saw in that video
- major optimization, the same way that cells interact with cells through the batched grid calculation method, should be used for cells with food

-----------------------------------------------------------------------

##### Simulation TODO
- if zoomed too far out you cant select on protozoa
- Add ambient music with mild bubble sound effects
- std::cout debug prints in production code - Add a constexpr bool DEBUG_LOGGING = false flag in settings and gate all std::cout behind it, or use a proper logger.
- orginize settings files

-----------------------------------------------------------------------

##### Rendering TODO
- Simulation should start more zoomed out
- a foods saturation or brightness should be preportional to how close it is to being able to reproduce
- When designing the cells, The outer and inner layers should be roughly the same color, the inside should be very transparent and the outside a lot less



-----------------------------------------------------------------------

#### Multithreading todo
- Get the seperate thread architecture fully working
- Check with previous version to see if there are any performance degradations
- Get the Rendering and Updating Working on Seperate Threads
- Separate the update thread into Several other worker threads
- BenchMark the performance with my old laptop
Multithreadding GUI:
- Tickbox to activate the debugger 
- Slider to adjust the number of worker threads on Updating
- Histogram or bar graph showing the load and memory usage of each thread

-----------------------------------------------------------------------

##### Protozoa TODO
- when a new cell is created it should have very low friction and generally not affect the organism too much, test this
- create a cell body class
- each protozoa stores cell_positions_nearby and food positions for debugging, just have it once in the protozoa manager
- when springs are created through create cell or create spring, give them random properties
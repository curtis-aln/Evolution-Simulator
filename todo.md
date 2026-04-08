----------------------------------------------------------------------
##### ImGUI TODO
- time elapsed in hours minutes seconds
- add infant mortality stat
- statistic showing total alive entities
- Death notifications: optional toast notification when a tagged organism dies, with cause of death (starvation, age, etc.
- Clone: duplicate an organism exactly, spawn the copy nearby
- Genome editor: directly tweak a selected organism's trait values via sliders, bypassing mutation. Great for controlled experiments
- Organism tagging, ability to tag an organism and it will be outlined and visible, will show up on the tagged organisms screen
- Add tabs for these IMgui screens because i imagine there will be a lot of them eventually and it will get cluttered, so maybe have one for statistics, one for controls, one for the tagged organisms, etc.
- Time-lapse / fast-forward mode — run simulation at 10x-100x with minimal rendering for long-run experiments
- Ability to make organisms immortal
- Ability to force organism to reproduce or die
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

-----------------------------------------------------------------------

##### world TODO
- Organisms usually span accross more than one cell, so you cant use their centre to determine where to look for food
- spatial hash grid should dynamically change density as the population changes, and should be able to change size too
- Add radiation zones which mutates an organism based on their proximity to the center of the zone, the closer they are the higher the mutation rate and range, but also the higher the energy cost for existing in that zone
- Add black holes which can pull in protozoa and food, but not kill them, just make them orbit until they get pulled out by another protozoa or food item
  can be modified by the user. if the gravitational pull is negative it becomes a white hole which pushes things away instead of pulling them in
- Add a layered background like you saw in that video
- major optimization, the same way that cells interact with cells through the batched grid calculation method, should be used for cells with food
- a foods saturation or brightness should be preportional to how close it is to being able to reproduce

-----------------------------------------------------------------------

##### Simulation TODO
- if zoomed too far out you cant select on protozoa
- figure out how to put rendering and updating on seperate threads
- Add ambient music with mild bubble sound effects
- Simulation should start more zoomed out
- std::cout debug prints in production code - Add a constexpr bool DEBUG_LOGGING = false flag in settings and gate all std::cout behind it, or use a proper logger.
- 
-----------------------------------------------------------------------

##### Protozoa TODO
- when a new cell is created it should have very low friction and generally not affect the organism too much, test this
- create a cell body class
- each protozoa stores cell_positions_nearby and food positions for debugging, just have it once in the protozoa manager

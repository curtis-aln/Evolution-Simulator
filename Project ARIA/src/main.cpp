#include "simulation/simulation.h"

//  05/03 - 922  lines (+0)
//  10/03 - 962  lines (+40)
//  17/03 - 1131 lines (+169)
//  20/03 - 1255 lines (+124)
//  21/03 - 1429 lines (+174)
//  23/03 - 1769 lines (+340)
//  24/03 - 2011 lines (+242) <-
//  27/03 - 2132 lines (+121)
//  02/04 - 2485 lines (+353)
//  05/04 - 2748 lines (+263)

// todo:
// [DONE] ability to toggle debug mode with an indicator of if you are in debug mode
// [DONE] bounding box for Protozoa, bounding box shown when organism is hovered over
// [DONE] cell count and age of organism displayed when hovering over
// [DONE] world space and bounds, ability to drag and move around world-space
// [DONE] zooming ability
// [DONE] multiple organisms existing at once
// [DONE] show connections between cells in debug mode with arrows
// [DONE] cell spring force working correctly
// [DONE] circular border, have it rendered
// [DONE] ability to select when zoomed in
// [DONE] revisit the cell generation alg so that every cell is connected in some way
// [DONE] add an init.cpp file to protozoa
// [DONE] ability to drag cells around in debug mode
// [DONE] show cell velocities in debug mode
// [DONE] cells stay in border
// [DONE] builder menu bounding box
// [DONE] create a button
// [DONE] thick organic looking lines for connections
// [DONE] cell & food population line graph
// [DONE] Add builder menu protozoa to world


// todo builder:
// TODO builder menu toggle
// TODO builder menu add / remove node
// TODO builder menu add / remove connection

// todo neural network:
// TODO Network visualiser
// TODO Network weight adder / remover
// TODO Network Node modification
// TODO Working Network for organisms with inputs and outputs working


// todo utils:
// TODO make rounded_rect.h
// TODO make text_box.h
// TODO create a slider
// TODO find cleaner and better fonts
// TODO smooth zooming
// TODO simple mode [S] which hides all utility accessories
// TODO button on the line graph to toggle between line mode and fill mode
// TODO turn springs into a module
// TODO have some objects static on the screen instead of on the world <- create a surface for the world
// TODO dynamic quad tree for organisms & food


// todo protozoa & food:
// TODO cell connections should change size based on extension
// TODO food clusters spawning
// TODO food population management
// TODO text for each node on the line mode+
// TODO protozoa starvation
// TODO protozoa reproduction
// TODO springs showing stress and strain
// TODO cells can grow in size over time
// TODO - Mitosis, Each cell duplicates, those new cells begin to cluster into a ball and form connections


int main()
{
	Simulation().run();
}


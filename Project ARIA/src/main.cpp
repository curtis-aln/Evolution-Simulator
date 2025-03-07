#include "simulation/simulation.h"


// NOTE: do not use cell id's as indexes as when cells are added and removed it can mess up
// NOTE: if there are multithreading issues check how rendering works with the update thread because i commented out the lock mutex

// ********** Currently Working On ********** //
// Create a RectBuffer and re-introduce the connections
// fix the flickering issue and include it as a setting
// fix the transparency issue
// re-introduce debug mode
// fix "(failed to create the font face)" bug
// fix movement bug
// connections are drawn ontop of the cells
// [BIG PROJECT] Add the ability to create and destroy protozoa

// ********** Debug mode ********** //
// upon clicking on a protozoa
// - the camera begins tracking the center of the organism
// - the camera is zoomed onto it



// Things ive noticed while Testing
// - make "project aria" title area larger and move it to the bottom right
// - fps should be integrated with the protozoa settings
// - program crashes when you try to link two protozoa in the builder area
// - make instructions more clear in builder area
// - program doesn't run from executable
// - statistics line and data-points are too small
// - statistics title is too large
// - connections between cells should be smoother
// - protozoa factory title too far down
// - neural network input colors too strong
// - cells need better creation colors, should fit a pallet like "lime-pallet" or "ocean-pallet"

// Things i should move onto
// 
// Cells should be able to bounce off each-over

// Features to add
// Food



// todo strings:
// TODO springs showing stress and strain in colour hue
// TODO springs should change size based on extension
// TODO turn springs into a module
// TODO make two debug arrows showing the direction of expansion / contraction
// TODO give those arrows values


// todo advanced:
// TODO dynamic quad tree for organisms & food
//      TODO ability to add Nodes
//      TODO ability to visualise tree
//      TODO ability to remove nodes
//      TODO ability to update_one_frame nodes
//      TODO ability to query
// TODO util and statistics on seperate thread
// TODO simulation rendering and physics on seperate threads
// TODO improve o_vector: each object points to the previous and next index. if removed then
//      TODO self.previous.next_index = self.next_index
//      TODO self.next.previous_index = self.previous_index
// TODO ability to create multiple worlds with multi-threading

// todo quality of life:
// TODO Remove the child and parent id information from cells
// TODO user should have a radius to select and drag multiple cells at a time

int main()
{
	Simulation().run();
}


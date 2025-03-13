#include "simulation/simulation.h"


// NOTE: do not use cell id's as indexes as when cells are added and removed it can mess up
// NOTE: if there are multithreading issues check how rendering works with the update thread because i commented out the lock mutex

// ********** Currently Working On ********** //
// Create a RectBuffer and re-introduce the connections
// fix "(failed to create the font face)" bug
// connections are drawn ontop of the cells which is wrong
// Intercellular collisions
// Multicellular collisions
// Advanced cell debug mode
// Improve border collisions, currently too aggressive


// Things ive noticed while Testing
// the grid renderers dont work with negative x values
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

// New collision resolution algorithm
// world collects all cells into a vector
// all collision resolution happens in world class
// border collision resolution happens there too
// also each collision is calculated twice

// current fps: 30
// after improvements: 60fps
// after collision slash: 82fps 

int main()
{
	///Random::set_seed(0);
	Simulation().run();
}


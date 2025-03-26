#include "simulation/simulation.h"


// NOTE: do not use cell id's as indexes as when cells are added and removed it can mess up
// NOTE: if there are multithreading issues check how rendering works with the update thread because i commented out the lock mutex

// ********** Currently Working On ********** //

/* Features */
// World size gradually increases
// A Dynamic Quad Tree with auto local and global resizing

/* Graphics */
// Create a RectBuffer and re-introduce the connections
// Cells should fade in and out of existance


/* Debug */
// fix "(failed to create the font face)" bug
// connections are drawn ontop of the cells which is wrong
// Advanced cell debug mode
// - the grid renderers dont work with negative x values
// - program doesn't run from executable


// - protozoa id should be shown
// - bounding box not working appropriately
// - all cells of teh same protzoa should have the same color
// - spring const, damping, and mutaiton constants should be mutatable

// NOTE: vibrations need an offset
// Experiment with constant time period for springs only
// Solve overpopulation issues by removing a random protoszoa

int main()
{
	///Random::set_seed(0);
	Simulation().run();
}


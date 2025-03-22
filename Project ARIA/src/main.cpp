#include "simulation/simulation.h"


// NOTE: do not use cell id's as indexes as when cells are added and removed it can mess up
// NOTE: if there are multithreading issues check how rendering works with the update thread because i commented out the lock mutex

// ********** Currently Working On ********** //

/* Features */
// World size gradually increases
// A Dynamic Quad Tree with auto local and global resizing

/* Graphics */
// Create a RectBuffer and re-introduce the connections


/* Debug */
// red lines from the center of mass to food
// fix "(failed to create the font face)" bug
// connections are drawn ontop of the cells which is wrong
// Advanced cell debug mode
// - the grid renderers dont work with negative x values
// - program doesn't run from executable


// - re-align the stats with the bounding box
// - energy value should be rounded
// - protozoa id should be shown
// - cell id remove "id" and just have the number
// - replace frames old with age
// - replace cell count with cells and springs
// - A better color scheme is needed
// - bounding box not working appropriately

int main()
{
	///Random::set_seed(0);
	Simulation().run();
}


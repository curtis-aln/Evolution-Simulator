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
// Statistics graphs should be constantly at the bottom right corner of the screen
// Food and cell counts should be on the same graph
// sort out recaling issues with the line graph
// add statistics statically on the screen aswell
// make a "draw_static" function for the camera


/* Debug */
// - the grid renderers dont work with negative x values
// - program doesn't run from executable
// - when selecting organisms, everything else should fade a bit

// - protozoa id should be shown
// - bounding box not working appropriately
// - display mutation rate and mutation range
// - track average mutation rate / mutation range
// - display offspring count and track average
// - display food eaten

// add o_vector compatability for the circleBuffer and spatial_hash_grid

int main()
{
	///Random::set_seed(0);
	Simulation().run();
}



// before, fps: 30
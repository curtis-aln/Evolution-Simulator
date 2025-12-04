#include "simulation/simulation.h"

/*
First Test Log
- Basic survivial was achived but no notable evolution, organisms remained as 3 cell worms.

problem and solutions:
- there should be a Minimum spring extension range to stop vibrating which i see as cheating
- i think cells avoid adding any new cells because they start messing up the organisms move patterns,
  when one is added they should have little to no forces acting on the other cells to begin with.
- color mutations happen too quickly - needs to be reduced quite significantly
- the gui range is wayy too small
- the cell to food preportion is off significantly
- make sure the mutation probabilities for everything is thought through and callibrated, 
  for example 
  - the chance of a connection getting deleted should be preportional to how weak it is
  - the chance for two cells to form a connection is negatively preportional to the distance of that cell
  - the probability of a cell getting removed is negtatively preportional to the ammount of connections it has

- we will also Create a function that goes over the deletion of any object which is preportional to time but gets clamped
  this is to help new body parts survive the "weak" stage we created without immidately being deleted
*/






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
// collision resolution improvements, fps: 38
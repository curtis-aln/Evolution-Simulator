#include "simulation/simulation.h"

//  05/03 - 922  lines (+0)
//  10/03 - 962  lines (+40)
//  17/03 - 1131 lines (+169)
//  20/03 - 1255 lines (+124)
//  21/03 - 1429 lines (+174)
//  23/03 - 1769 lines (+340)
//  24/03 - 2011 lines (+242)
//  27/03 - 2132 lines (+121)
//  02/04 - 2485 lines (+353)
//  05/04 - 2748 lines (+263)
//  09/04 - 3253 lines (+505)
//  12/04 - 3656 lines (+403)


// todo builder:
// TODO builder menu toggle
// TODO builder menu add / remove node
// TODO builder menu add / remove connection

// todo neural network:
// TODO Network weight adder / remover
// TODO Network Node modification
// TODO Working Network for organisms with inputs and outputs working
// TODO see weights and bias values

// todo utils:
// TODO create a slider
// TODO find cleaner and better fonts
// TODO smooth zooming
// TODO simple mode [S] which hides all utility accessories
// TODO button on the line graph to toggle between line mode and fill mode
// TODO turn springs into a module
// TODO have some objects static on the screen instead of on the world <- create a surface for the world
 
// todo line graph:
// TODO add range for y-axis in graph
// TODO add x-axis value indicators  

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

// todo advanced:
// TODO dynamic quad tree for organisms & food
//      TODO ability to add Nodes
//      TODO ability to visualise tree
//      TODO ability to remove nodes
//      TODO ability to update nodes
//      TODO ability to query
// TODO util and statistics on seperate thread
// TODO simulation rendering and physics on seperate threads
// TODO improve o_vector: each object points to the previous and next index. if removed then
//      TODO self.previous.next_index = self.next_index
//      TODO self.next.previous_index = self.previous_index

// todo quality of life:
// TODO Remove the child and parent id information from cells
// TODO fix protozoa dragging issue

int main()
{
	Simulation().run();
}


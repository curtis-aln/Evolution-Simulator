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
//  15/04 - 3958 lines (+302)


// NOTE: do not use cell id's as indexes as when cells are added and removed it can mess up

// togo bugs:
// TODO cells bugging out
// TODO cells not fully connected

// todo builder:
// TODO builder menu add / remove node
// TODO builder menu add / remove connection
// TODO ability to release node
// TODO fix node hiding bug


// todo neural network:
// TODO Working Network for organisms with inputs and outputs working
// TODO see weights and bias values
// TODO natural selection alg working
// TODO mutation and evolution alg


// todo utils:
// TODO create optimized vector
// TODO create a slider
// TODO find cleaner and better fonts
// TODO smooth zooming
// TODO simple mode [S] which hides all utility accessories
// TODO have some objects static on the screen instead of on the world <- create a surface for the world
 
// todo line graph:
// [DONE] add range for y-axis in graph
// [DONE] add x-axis value indicators
// [DONE] fix bug where values at the end are not showing.
// [DONE] add line graph toggle to see the line graph
// TODO add stats on the line graph

// todo food:
// TODO food clusters spawning
// TODO food population management
// TODO food cluster locomotion
// TODO food debug settings


// todo protozoa:
// TODO working neural network
// TODO cell eating system
// TODO protozoa energy system
// TODO protozoa reproduction
// TODO protozoa death


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
//      TODO ability to update nodes
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


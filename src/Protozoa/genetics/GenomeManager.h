#pragma once

#include "../cell.h"
#include "../spring.h"


#include <vector>


class GenomeManager
{
	std::vector<Cell>* cells_;
	std::vector<Spring>* springs_;


public:
    GenomeManager(std::vector<Cell>* cells, std::vector<Spring>* springs)
        : cells_(cells), springs_(springs)
    {

	}
};

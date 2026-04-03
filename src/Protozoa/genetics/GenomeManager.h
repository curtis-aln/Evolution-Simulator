#pragma once

#include "../../Utils/Random.h"
#include  "../../Utils/Graphics/OrganicColorMutator.h"

#include "../cell.h"
#include "../spring.h"


#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>


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

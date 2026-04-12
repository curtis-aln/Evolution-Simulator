#pragma once

#include <cstdint>
#include <iostream>
#include <array>

#include "fixed_span.h"

// Can peroformance be gained through telling the object what grid id they are?


using cell_idx = uint32_t;
using obj_idx = uint32_t;

struct SimpleSpatialGrid
{
	size_t CellsX = 0;
	size_t CellsY = 0;
	size_t cell_max_capacity = 0;

	float cell_width = 0;
	float cell_height = 0;

	float world_width = 0;
	float world_height = 0;

	using CellArray = std::vector<obj_idx>;
	alignas(32) std::vector<CellArray> grid{}; // An array of cells, each cell is an array full of object ids
	alignas(32) std::vector<uint8_t> cell_capacities{}; // Tracking the size of each cell, avoiding using pushback() and removal operations


public:
	explicit SimpleSpatialGrid(size_t Cells_X, size_t Cells_Y, int cellCapacity, float worldWidth, float worldHeight)
		: CellsX(Cells_X), CellsY(Cells_Y), cell_max_capacity(cellCapacity), world_width(worldWidth), world_height(worldHeight)
	{
		// creating the cell sizes grid
		cell_capacities.resize(CellsX * CellsY, 0);

		// creating the object container grid
		grid.resize(CellsX * CellsY, CellArray());
		for (size_t i = 0; i < CellsX * CellsY; ++i)
			grid[i].resize(cell_max_capacity, 0);
		

		update_cell_dimensions();
	}
	~SimpleSpatialGrid() = default;


	void update_cell_dimensions()
	{
		cell_width = world_width / static_cast<float>(CellsX);
		cell_height = world_height / static_cast<float>(CellsY);
	}

	void change_cell_dimsensions(const int new_cells_x, const int new_cells_y)
	{
		CellsX = new_cells_x;
		CellsY = new_cells_y;
		update_cell_dimensions();

		cell_capacities.resize(CellsX * CellsY, 0);

		grid.resize(CellsX * CellsY, CellArray());
		for (size_t i = 0; i < CellsX * CellsY; ++i)
			grid[i].resize(cell_max_capacity, 0);
	}

	cell_idx inline hash(const float x, const float y) const
	{
		// Converting the position to a 2d grid coordinate
		const auto cell_x = static_cast<cell_idx>(x / cell_width);
		const auto cell_y = static_cast<cell_idx>(y / cell_height);

		// pressing the 2d grid coordinate into a 1d index for cashe efficiency
		return cell_y * CellsX + cell_x;
	}


	// adding an object to the spatial hash render_grid_ by a position and storing its obj_id
	cell_idx inline add_object(const float x, const float y, const size_t obj_id)
	{
		const cell_idx index = hash(x, y);

		// fetching the current capacity of the cell, which can be used as the index to add the new object
		uint8_t& cell_capacity = cell_capacities[index];

		// adding the objec to the cell
		grid[index][cell_capacity] = static_cast<obj_idx>(obj_id);
		
		// increasing the cells capacity only if it is not already at maximum, otherwise we just drop the object on the floor
		cell_capacity += cell_capacity < cell_max_capacity - 1;

		return index;
	}

	void clear()
	{
		// every cells capacity is set to zero allowing them to be overwritten
		for (int idx = 0; idx < CellsX * CellsY; ++idx)
		{
			cell_capacities[idx] = 0;
		}
	}

	size_t get_total_cells() const { return CellsX * CellsY; }

	// This fuction returns 9 cell contents, the cell the position is in and the 8 cells surrounding it, which is used for collision detection
	void find(const float x, const float y, FixedSpan<obj_idx>* const container)
	{
		find_from_index(hash(x, y), container);
	}

	void find_from_index(const cell_idx index, FixedSpan<obj_idx>* const container)
	{
		container->count = 0;
		const int cell_x = index % CellsX;
		const int cell_y = index / CellsX;
		// for each grid cell sorrounding the cell, including the cell
		for (int nx = cell_x - 1; nx <= cell_x + 1; ++nx)
		{
			for (int ny = cell_y - 1; ny <= cell_y + 1; ++ny)
			{
				// if this cell lies outside of the grid, we skip it
				if (nx < 0 || ny < 0 || nx >= static_cast<int>(CellsX) || ny >= static_cast<int>(CellsY))
					continue;
				// 2d -> 2d index conversion
				const cell_idx neighbour_index = ny * CellsX + nx;
				// fetching the number of objects in this cell, which is the number of objects we need to add to the container
				const uint8_t count = cell_capacities[neighbour_index];
				for (uint8_t i = 0; i < count; ++i) // Potential Optimization here
					container->add(grid[neighbour_index][i]);
			}
		}
	}

	void track_stats(int& total, int& max_in, int& full, int& empty, int& inv)
	{
		for (size_t i = 0; i < get_total_cells(); ++i)
		{
			const int c = static_cast<int>(cell_capacities[i]);
			total += c;
			if (c == 0)                                         ++empty;
			if (c >= static_cast<int>(cell_max_capacity)) ++full;
			if (c > max_in)                                     max_in = c;
		}

		const size_t tc = get_total_cells();
		inv = tc > 0 ? 1.f / static_cast<float>(tc) : 0.f;

		ImGui::Spacing();
		ImGui::Text("Objects  %d", total);
		ImGui::Text("Avg/cell %.2f", tc > 0 ? static_cast<float>(total) * inv : 0.f);
		ImGui::Text("Max cell %d  (%.0f%%)", max_in,
			cell_max_capacity > 0 ? max_in * 100.f / static_cast<float>(cell_max_capacity) : 0.f);
		ImGui::Text("Full     %d  (%.1f%%)", full, full * 100.f * inv);
		ImGui::Text("Empty    %d  (%.1f%%)", empty, empty * 100.f * inv);

		if (full > 0)
			ImGui::TextColored({ 1.f, 0.4f, 0.4f, 1.f },
				"[!] %d at cap — objects may drop", full);
	}
};
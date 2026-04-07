#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "simple_spatial_grid.h"


template<size_t CellsX, size_t CellsY, size_t cell_max_capacity>
class SimpleSpatialGridRenderer
{
	using GridType = SimpleSpatialGrid<CellsX, CellsY, cell_max_capacity>;
	GridType* grid;	

	sf::VertexBuffer vertexBuffer{};
	sf::Font font;
	sf::Text text;

public:
	SimpleSpatialGridRenderer(GridType* grid)
	{
		initFont();
		initVertexBuffer();
	}


	void render_grid(sf::RenderWindow& window)
	{
		window.draw(vertexBuffer);

		// rendering the locations of each cell with their content counts
		for (int x = 0; x < CellsX; ++x)
		{
			for (int y = 0; y < CellsY; ++y)
			{
				const cell_idx index = y * CellsX + x;
				const sf::Vector2f topleft = { x * cell_dimensions_.x, y * cell_dimensions_.y };
				text.setString("(" + std::to_string(x) + ", " + std::to_string(y) + ")  obj count: " + std::to_string(objects_count[index]));
				text.setPosition(topleft);
				window.draw(text);
			}
		}
	}


private:
	void initVertexBuffer()
	{
		std::vector<sf::Vertex> vertices((CellsX + CellsY) * 2);

		vertexBuffer = sf::VertexBuffer(sf::PrimitiveType::Lines, sf::VertexBuffer::Usage::Static);
		vertexBuffer.create(vertices.size());

		size_t counter = 0;
		for (size_t x = 0; x < CellsX; x++)
		{
			const float posX = static_cast<float>(x) * cell_dimensions_.x;
			vertices[counter].position = { posX, 0 };
			vertices[counter + 1].position = { m_screenSize.position.x + posX, m_screenSize.position.y + m_screenSize.size.y };
			counter += 2;
		}

		for (size_t y = 0; y < CellsY; y++)
		{
			const float posY = static_cast<float>(y) * cell_dimensions_.y;
			vertices[counter].position = { 0, posY };
			vertices[counter + 1].position = { m_screenSize.position.x + m_screenSize.size.x, m_screenSize.position.y + posY };
			counter += 2;
		}

		for (size_t x = 0; x < counter; x++)
		{
			vertices[x].color = { 75, 75, 75 };
		}

		vertexBuffer.update(vertices.data(), vertices.size(), 0);
	}

	void initFont()
	{
		constexpr int char_size = 45;
		const std::string font_location = "media/fonts/Calibri.ttf";
		if (!font.openFromFile(font_location))
		{
			std::cerr << "[ERROR]: Failed to load font from: "
				<< font_location << '\n';
		}
	}
};
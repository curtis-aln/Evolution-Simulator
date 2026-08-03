#pragma once
#include "cell/cell.h"

struct CellMatter
{
	static constexpr float    friction = 0.85f;
	static constexpr uint16_t max_time_to_live = 500; // frames

	static constexpr uint8_t max_radius = 70;
	static constexpr uint8_t min_radius = 40;

	static constexpr uint8_t max_gray = 70;
	static constexpr uint8_t min_gray = 40;

	static constexpr float spawn_chance = 0.75f;

	bool dead = false;

	uint16_t id_ = 0;
	uint16_t body_id_ = 0;

	uint8_t  inner_gray_ = 0;
	uint8_t  outer_gray_ = 0;
	uint16_t time_alive = 0;

	sf::Color inner_color() const { return sf::Color(inner_gray_, inner_gray_, inner_gray_); }
	sf::Color outer_color() const { return sf::Color(outer_gray_, outer_gray_, outer_gray_); }

	void update(Body* body)
	{
		time_alive++;
		body->velocity_ *= friction;

		dead = time_alive > max_time_to_live;
	}

	void reset_cell_manager()
	{
		time_alive = 0;
	}

	void cell_to_matter(Body* body)
	{
		inner_gray_ = Random::rand_range<uint8_t>(min_gray, max_gray);
		outer_gray_ = Random::rand_range<uint8_t>(min_gray, max_gray);

		time_alive = 0;

		body->radius_ = Random::rand_range<uint8_t>(min_radius, max_radius);

		body_id_ = body->id_;
	}
};
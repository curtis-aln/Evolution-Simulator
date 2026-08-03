#pragma once

/* Body class is a simple struct which holds the physical properties of a cell, such as position, velocity, and mass. */

using BodyId = uint32_t;


struct Body
{
	bool active = true;

	// This id references the body itself
	BodyId id_ = 0;

	// Keeping track of nearby id's to resolve collisions with
	std::array<BodyId, 8> nearby_ids_;
	uint8_t nearby_ids_size_ = 0;

	inline static constexpr float density = 1.f; // the density of the cell, used to calculate radius from mass

	sf::Vector2f position_;
	sf::Vector2f velocity_;
	float mass_;
	float radius_;

	Body(BodyId id = 0)
		: id_(id)
	{
		
	}

	void accelerate(const sf::Vector2f& acceleration)
	{
		velocity_ += acceleration;
	}

	void update_physics()
	{
		position_ += velocity_;
	}

	void reset_cell_manager()
	{
		velocity_ = { 0.f, 0.f };
		nearby_ids_size_ = 0;
	}

	void copy(const Body* other)
	{
		position_ = other->position_;
		velocity_ = other->velocity_;
		mass_ = other->mass_;
		radius_ = other->radius_;
		nearby_ids_size_ = 0;
	}

};
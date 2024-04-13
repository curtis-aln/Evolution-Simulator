#pragma once
#include <array>

template<class T, unsigned max_quantity>
class Vector
{
	// The array holds all the actual information of the classes 
	std::array<T, max_quantity> m_array_{};

	// controls how to iterate through m_array. std::pair<before index, after index>
	std::array<std::pair<unsigned, unsigned>, max_quantity> m_connectors_{};

	unsigned m_size_ = 0u;
	unsigned m_starting_index_ = 0;



public:
	Vector()
	{
		
	}

	T* add()
	{
		
	}

	bool remove(const int id)
	{
		if (m_size_ == 0)
			return false;


	}

};

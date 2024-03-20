#pragma once

#include <chrono>

// a simple class used for managing time frames, the GetDelta function updates the time so call it at set frequent intervals
struct DeltaTime
{
	DeltaTime()
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	double GetDelta()
	{
		const auto currentTime = std::chrono::high_resolution_clock::now();
		const auto delta = currentTime - m_start;
		m_start = currentTime;
		return std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();
	}

private:
	std::chrono::high_resolution_clock::time_point m_start;
};
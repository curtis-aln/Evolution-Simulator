#pragma once


// a class used to get average framerates
template<const unsigned resolution>
struct BetterFrameRates
{
	unsigned framerates[resolution] = {};
	unsigned size = 0;
	unsigned counter = 0;

	int getFrameRate()
	{
		if (size == 0) return 0;

		float sum = 0;
		for (const unsigned val : framerates) { sum += val; }
		return static_cast<int>(sum / size);
	}

	void updateFrameRates(const unsigned frameRate)
	{
		if (counter >= resolution)
			counter = 0;

		if (size < resolution) ++size;
		framerates[counter++] = frameRate;
	}
};

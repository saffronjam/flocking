#pragma once
#include <stdlib.h>
#include <time.h>

class Random
{

public:
	Random(int const &lower = 0, int const &upper = 10)
		: m_lower(lower),
		  m_upper(upper){};

	int Gen() { return (double)((rand() % ((int)m_upper - (int)m_lower)) + (int)m_lower); }

	void SetLowerBound(int lowerBound_IN) { m_lower = lowerBound_IN; };
	void SetUpperBound(int upperBound_IN) { m_upper = upperBound_IN; };

	static double Generate(double lower, double upper)
	{
		if (lower > upper)
		{
			std::swap(lower, upper);
		}
		return (double)((rand() % ((int)upper - (int)lower)) + (int)lower);
	}

private:
	int m_lower;
	int m_upper;
};
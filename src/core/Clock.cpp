#include "Clock.h"

sf::Clock Clock::m_clock;
sf::Time Clock::m_dt = sf::seconds(1.0f / 60.0f);
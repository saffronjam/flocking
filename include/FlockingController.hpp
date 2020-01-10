#pragma once

#include <memory>
#include "Boid.hpp"

class FlockingController
{
public:
    FlockingController(int const &nBoids);

    void Update(sf::Time const &dt);
    void Draw(Graphics &gfx);

private:
    void Flock();
    void ComputeBoidsVisibleNeighbors();

private:
    std::vector<std::shared_ptr<Boid>> m_allBoids;
};

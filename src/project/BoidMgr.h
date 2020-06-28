#pragma once

#include "Boid.h"
#include "Random.h"

class BoidMgr
{
public:
    BoidMgr();

    void Update();
    void Draw();

    void SetBoidCount(size_t count);

private:
    void CalculateAllVisibleNeighbors();

private:
    std::vector<Boid> m_boids;
};
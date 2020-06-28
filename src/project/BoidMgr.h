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

    void SetSeparationMultiplier(float multiplier) noexcept;
    void SetAlignmentMultiplier(float multiplier) noexcept;
    void SetCohesionMultiplier(float multiplier) noexcept;

private:
    void CalculateAllVisibleNeighbors();
    void ClampBoidsToSimulationBox();
    sf::Vector2f GetRepulsionBorderForce(const Boid &boid) const noexcept;

private:
    std::vector<Boid> m_boids;

    sf::FloatRect m_simulationBox;
    sf::FloatRect m_repulsionBorders;

    // Cached
    float m_separationMultiplier;
    float m_alignmentMultiplier;
    float m_cohesionMultiplier;
};
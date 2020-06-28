#pragma once

#include <unordered_set>

#include "Boid.h"
#include "Random.h"

class BoidMgr
{
public:
    BoidMgr();

    void Update();
    void Draw();
    void DrawQuadTree();

    void SetBoidCount(size_t count);

    void SetSeparationMultiplier(float multiplier) noexcept;
    void SetAlignmentMultiplier(float multiplier) noexcept;
    void SetCohesionMultiplier(float multiplier) noexcept;

private:
    void CalculateAllVisibleNeighbors();
    sf::Vector2f GetRepulsionBorderForce(const Boid &boid) const noexcept;

    void ComputeQuadTree();
    void ClearQuadtree() noexcept;

private:
    std::vector<Boid> m_boids;
    std::vector<std::vector<std::vector<Boid *>>> m_quadtree;
    std::set<std::pair<size_t, size_t>> m_activeContainers;
    sf::FloatRect m_quadtreeRect;
    sf::FloatRect m_quadtreeBox;

    sf::FloatRect m_repulsionBorders;
};
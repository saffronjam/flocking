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
    void DrawFlockConvexHull();

    void SetBoidCount(size_t count);

    void SetSeparationMultiplier(float multiplier) noexcept;
    void SetAlignmentMultiplier(float multiplier) noexcept;
    void SetCohesionMultiplier(float multiplier) noexcept;

    void SetMinSpeed(float speed) noexcept;
    void SetMaxSpeed(float speed) noexcept;

    void SetSightRadius(float radius) noexcept;
    void SetSightAngle(float radius) noexcept;

    void SetDrawBody(bool onoff) noexcept { m_drawBody = onoff; }
    void SetDrawVision(bool onoff) noexcept { m_drawVision = onoff; }
    void SetDrawNeighbors(bool onoff) noexcept { m_drawNeighbors = onoff; }
    void SetDrawVelocity(bool onoff) noexcept { m_drawVelocity = onoff; }
    void SetDrawAcceleration(bool onoff) noexcept { m_drawAcceleration = onoff; }
    void SetDrawFlocks(bool onoff) noexcept { m_drawFlocks = onoff; }
    void SetDrawQuadtree(bool onoff) noexcept { m_drawQuadtree = onoff; }

private:
    void ComputeAllVisibleNeighbors();
    void ComputeFlocks();
    void IterativeFlockCheck(const Boid &boid, std::set<Boid *> &currentFlock);
    sf::Vector2f GetRepulsionBorderForce(const Boid &boid) const;

    void ComputeQuadTree();
    void ClearQuadtree() noexcept;

private:
    std::vector<Boid> m_boids;
    std::vector<std::set<Boid *>> m_flocks;
    std::vector<std::vector<std::vector<Boid *>>> m_quadtree;
    std::set<std::pair<size_t, size_t>> m_activeContainers;
    sf::FloatRect m_quadtreeRect;
    sf::FloatRect m_quadtreeBox;
    sf::VertexArray m_quadtreeGrid;

    sf::FloatRect m_repulsionBorders;

    bool m_drawBody;
    bool m_drawVision;
    bool m_drawNeighbors;
    bool m_drawVelocity;
    bool m_drawAcceleration;
    bool m_drawFlocks;
    bool m_drawQuadtree;
};
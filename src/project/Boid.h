#pragma once

#include <set>

#include "VoidBody.h"
#include "Camera.h"

class Boid
{
public:
    Boid(const sf::Vector2f &position);

    bool operator==(const Boid &otherBoid) { return m_voidBody == otherBoid.m_voidBody; }

    void DrawBody() const;
    void DrawSight() const;
    void DrawNeighbors() const;
    void DrawVisibleNeighbors() const;
    void DrawVelocity() const;
    void DrawAcceleration() const;

    void AddNeighbor(const Boid *boid) { m_neighbors.emplace(boid); }
    void AddVisibleNeighbor(const Boid *boid) { m_visibleNeighbors.emplace(boid); }
    void ClearNeighbors() noexcept { m_neighbors.clear(); }
    void ClearVisibleNeighbors() noexcept { m_visibleNeighbors.clear(); }

    void ApplyForce(const sf::Vector2f &force) noexcept;

    sf::Vector2f GetPosition() const noexcept { return vl::ConvertTo<sf::Vector2f>(m_voidBody->GetBody()->GetPosition()); }
    float GetSightRadius() const noexcept { return m_sightRadius; }
    float GetSightAngle() const noexcept { return m_sightAngle; }
    sf::Vector2f GetForward() const noexcept;
    std::pair<sf::Vector2f, sf::Vector2f> GetSightBounds() const;

private:
    VoidBody::Ptr m_voidBody;

    std::set<const Boid *> m_neighbors;
    std::set<const Boid *> m_visibleNeighbors;

    sf::Color m_bodyColor;

    float m_sightRadius;
    float m_sightAngle;
};
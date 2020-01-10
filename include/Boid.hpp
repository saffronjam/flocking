#pragma once

#include <set>
#include <memory>

#include "Graphics.hpp"

class Boid
{
public:
    Boid(sf::Vector2f position, sf::Vector2f startVelocity, float collideZoneRadius);

    void Update(sf::Time const &dt);
    void Draw(Graphics &gfx);
    void DrawLineToNeighbors(Graphics &gfx);
    void DrawCollideZoneCircle(Graphics &gfx);
    void DrawVisionShape(Graphics &gfx);

    void Clamp(sf::FloatRect const &box);

    sf::Vector2f const &GetPosition() { return m_position; }
    sf::Vector2f const &GetVelocity() { return m_velocity; }
    float GetSeeingDistance() { return m_seeingDistance; }
    float GetSeparationStrength() { return m_separationStrength; }
    float GetAlignmentStrength() { return m_alignmentStrength; }
    float GetCohesionStrength() { return m_cohesionStrength; }
    void SetAcceleration(sf::Vector2f const &acceleration) { m_acceleration = acceleration; }

    void AddNeighbor(std::shared_ptr<Boid> boid) { m_neighbors.emplace(boid); }
    void RemoveNeighbor(std::shared_ptr<Boid> boid) { m_neighbors.erase(boid); }
    std::set<std::shared_ptr<Boid>> GetNeighbors() { return m_neighbors; }

    sf::Vector2f Separation();
    sf::Vector2f Alignment();
    sf::Vector2f Cohesion();

private:
    void CorrectBodyAccordingToDirection();

private:
    sf::Vector2f m_position;

    sf::Vector2f m_velocity;
    sf::Vector2f m_acceleration;

    sf::ConvexShape m_body;
    sf::CircleShape m_collideZoneRadiusShape;
    float m_collideZoneRadius;

    float m_seeingDistance;
    float m_seeingAngle;

    float m_separationStrength;
    float m_alignmentStrength;
    float m_cohesionStrength;

    std::set<std::shared_ptr<Boid>> m_neighbors;
};
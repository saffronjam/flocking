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
    void DrawLineToVisibleNeighbors(Graphics &gfx);
    void DrawVisionShape(Graphics &gfx);

    void Clamp(sf::FloatRect const &box);

    sf::Vector2f GetPosition() const { return m_position; }
    sf::Vector2f GetVelocity() { return m_velocity; }
    sf::Vector2f GetDirectionVector() const { return m_directionVector; }
    float GetSeeingDistance() const { return m_seeingDistance; }
    float GetSeeingAngle() const { return m_seeingAngle; }
    float GetSeparationStrength() const { return m_separationStrength; }
    float GetAlignmentStrength() const { return m_alignmentStrength; }
    float GetCohesionStrength() const { return m_cohesionStrength; }
    float GetSpeed() const { return m_speed; }
    std::pair<sf::Vector2f, sf::Vector2f> GetVisionLimitBorders() const;
    bool GetInFlock() const { return m_inFlock; }

    void SetAcceleration(sf::Vector2f const &acceleration) { m_acceleration = acceleration; }
    void SetSeeingDistance(float const &seeingDistance) { m_seeingDistance = seeingDistance; }
    void SetSeeingAngle(float const &seeingAngle) { m_seeingAngle = seeingAngle; }
    void SetSeparationStrength(float const &separationStrength) { m_separationStrength = separationStrength; }
    void SetAlignmentStrength(float const &alignmentStrength) { m_alignmentStrength = alignmentStrength; }
    void SetCohesionStrength(float const &cohesionStrength) { m_cohesionStrength = cohesionStrength; }
    void SetSpeed(float const &speed) { m_speed = speed; }
    void SetInFlock(bool const &inFlock) { m_inFlock = inFlock; }

    void AddNeighbor(std::shared_ptr<Boid> const &boid) { m_neighbors.emplace(boid); }
    void RemoveNeighbor(std::shared_ptr<Boid> const &boid) { m_neighbors.erase(boid); }
    void AddVisibleNeighbor(std::shared_ptr<Boid> const &boid) { m_visibleNeighbors.emplace(boid); }
    void RemoveVisibleNeighbor(std::shared_ptr<Boid> const &boid) { m_visibleNeighbors.erase(boid); }
    std::set<std::shared_ptr<Boid>> GetNeighbors() { return m_neighbors; }
    std::set<std::shared_ptr<Boid>> GetVisibleNeighbors() { return m_visibleNeighbors; }

    sf::Vector2f Separation();
    sf::Vector2f Alignment();
    sf::Vector2f Cohesion();

private:
    void UpdateDirectionVector();
    void CorrectBodyAccordingToDirection();
    void CorrectVisionAccordingToDirection();

private:
    sf::Vector2f m_position;

    sf::Vector2f m_velocity;
    sf::Vector2f m_acceleration;

    sf::Vector2f m_directionVector;

    sf::ConvexShape m_body;
    const int m_visionShapeMaxPoints;
    sf::VertexArray m_visionShape;

    float m_collideZoneRadius;

    float m_seeingDistance;
    float m_seeingAngle;
    float m_separationStrength;
    float m_alignmentStrength;
    float m_cohesionStrength;
    float m_speed;

    std::set<std::shared_ptr<Boid>> m_neighbors;
    std::set<std::shared_ptr<Boid>> m_visibleNeighbors;
    bool m_inFlock;
};
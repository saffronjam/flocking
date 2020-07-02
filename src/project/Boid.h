#pragma once

#include <set>

#include "VoidBody.h"
#include "Camera.h"
#include "Random.h"

class Boid
{
public:
    Boid(const sf::Vector2f &position);

    bool operator==(const Boid &otherBoid) { return m_position == otherBoid.m_position; }

    void Update();

    void DrawBody() const;
    void DrawSight() const;
    void DrawNeighbors() const;
    void DrawVisibleNeighbors() const;
    void DrawVelocity() const;
    void DrawAcceleration() const;

    sf::Vector2f GetSeparationForce() const;
    sf::Vector2f GetAlignmentForce() const;
    sf::Vector2f GetCohesionForce() const;

    void AddNeighbor(const Boid *boid) { m_neighbors.emplace(boid); }
    void AddVisibleNeighbor(const Boid *boid) { m_visibleNeighbors.emplace(boid); }
    void ClearNeighbors() noexcept { m_neighbors.clear(); }
    void ClearVisibleNeighbors() noexcept { m_visibleNeighbors.clear(); }
    std::set<const Boid *> GetNeighbors() const noexcept { return m_neighbors; }
    std::set<const Boid *> GetVisibleNeighbors() const noexcept { return m_visibleNeighbors; }

    void ApplyForce(const sf::Vector2f &force) noexcept;
    void ResetForce() noexcept { m_acceleration = vl::Null<>(); }

    const sf::Vector2f &GetPosition() const noexcept { return m_position; }
    float GetSightRadius() const noexcept { return m_sightRadius; }
    float GetSightAngle() const noexcept { return m_sightAngle; }
    sf::Vector2f GetForward() const noexcept;
    std::pair<sf::Vector2f, sf::Vector2f> GetSightBounds() const;
    sf::Vector2f GetVelocity() const noexcept { return m_velocity; };
    sf::Vector2f GetAcceleration() const noexcept { return m_acceleration; };
    float GetMaxSpeed() const noexcept { return m_maxSpeed; }
    float GetSeparationMultiplier() const noexcept { return m_separationMultiplier; }
    float GetAlignmentMultiplier() const noexcept { return m_alignmentMultiplier; }
    float GetCohesionMultiplier() const noexcept { return m_cohesionMultiplier; }
    sf::FloatRect GetBoundingBox() { return m_bodyShape.getLocalBounds(); }
    bool InFlock() const noexcept { return m_inFlock; }

    void SetPosition(const sf::Vector2f &position) noexcept { m_position = position; }
    void SetSightRadius(float radius) noexcept;
    void SetSightAngle(float angle) noexcept;
    void SetMinSpeed(float speed) noexcept { m_minSpeed = speed; }
    void SetMaxSpeed(float speed) noexcept { m_maxSpeed = speed; }
    void SetSeparationMultiplier(float multiplier) noexcept { m_separationMultiplier = multiplier; }
    void SetAlignmentMultiplier(float multiplier) noexcept { m_alignmentMultiplier = multiplier; }
    void SetCohesionMultiplier(float multiplier) noexcept { m_cohesionMultiplier = multiplier; }
    void SetInFlock(bool inFlock) const noexcept { m_inFlock = inFlock; }

private:
    void ReconstructVisionShape() noexcept;

private:
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    sf::Vector2f m_acceleration;

    float m_minSpeed;
    float m_maxSpeed;
    float m_maxForce;

    float m_separationMultiplier;
    float m_alignmentMultiplier;
    float m_cohesionMultiplier;

    std::set<const Boid *> m_neighbors;
    std::set<const Boid *> m_visibleNeighbors;

    sf::Color m_bodyColor;

    float m_sightRadius;
    float m_sightAngle;

    sf::Vector2f m_forward;

    mutable bool m_inFlock;

    sf::VertexArray m_visionShape;
    sf::ConvexShape m_bodyShape;
};
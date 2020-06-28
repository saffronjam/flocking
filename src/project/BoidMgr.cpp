#include "BoidMgr.h"

BoidMgr::BoidMgr()
{
    SetBoidCount(50);
    for (auto &boid : m_boids)
    {
        sf::Vector2f randomForce;
        while ((randomForce = Random::Vec2(-1.0f, -1.0f, 1.0f, 1.0f) * 1000.0f) == vl::Null<>())
            ;
        boid.ApplyForce(randomForce);
    }
}

void BoidMgr::Update()
{
    CalculateAllVisibleNeighbors();
    for (const auto &boid : m_boids)
    {
    }
}

void BoidMgr::Draw()
{
    for (const auto &boid : m_boids)
    {
        boid.DrawBody();
        // boid.DrawNeighbors();
        boid.DrawVisibleNeighbors();
        boid.DrawSight();
        boid.DrawVelocity();
    }
}

void BoidMgr::CalculateAllVisibleNeighbors()
{
    for (auto &boid : m_boids)
    {
        boid.ClearNeighbors();
        boid.ClearVisibleNeighbors();
        for (auto &otherBoid : m_boids)
        {
            if (boid == otherBoid)
                continue;

            const sf::Vector2f _thisPos = boid.GetPosition();
            const sf::Vector2f _otherPos = otherBoid.GetPosition();
            const sf::Vector2f forward = boid.GetForward();
            const auto sightBounds = boid.GetSightBounds();

            const bool inSeeingDistance = vl::Distance(_thisPos, _otherPos) < boid.GetSightRadius();
            const bool inPeripheral = !vl::IsLeft(_thisPos, sightBounds.first, _otherPos) && vl::IsLeft(_thisPos, sightBounds.second, _otherPos);
            const bool inFront = vl::IsLeft(vl::Perpendicular(forward), _thisPos, _otherPos);
            const bool angleBiggerThan180 = boid.GetSightAngle() > 180.0f;
            const bool angleBiggerThan360 = boid.GetSightAngle() > 360.0f;

            if (inSeeingDistance)
            {
                boid.AddNeighbor(&otherBoid);
            }

            if (inSeeingDistance && (inPeripheral ||
                                     (angleBiggerThan180 && inFront) ||
                                     (angleBiggerThan360)))
            {
                boid.AddVisibleNeighbor(&otherBoid);
            }
        }
    }
}

void BoidMgr::SetBoidCount(size_t count)
{
    while (m_boids.size() != count)
    {
        if (m_boids.size() < count)
        {
            sf::Vector2f randomPosition = Random::Vec2(-Camera::GetOffset(), sf::Vector2f(Window::GetWidth() - 200.0f, Window::GetHeight()));
            m_boids.push_back(Boid(randomPosition));
        }
        else
        {
            m_boids.pop_back();
        }
    }
}
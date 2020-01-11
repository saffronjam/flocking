#include "FlockingController.hpp"

#include "Random.hpp"
#include "Camera.hpp"
#include "Math.hpp"

FlockingController::FlockingController(int const &nBoids)
    : m_seeingDistance(100.0f),
      m_seeingAngle(Math::Constants::PI / 2.0f),
      m_separationStrength(1.0f),
      m_alignmentStrength(1.0f),
      m_cohesionStrength(1.0f),
      m_speed(50.0f),
      m_drawBoids(true),
      m_drawNeighborLines(false),
      m_drawVision(false)
{
    for (int i = 0; i < nBoids; i++)
    {
        sf::Vector2f randPos(Random::Generate(-Camera::OffsetX, Camera::OffsetX), Random::Generate(-Camera::OffsetY, Camera::OffsetY));
        sf::Vector2f randVel;
        do
        {
            randVel = sf::Vector2f(Random::Generate(-10.0f, 10.0f), Random::Generate(-10.0f, 10.0f));
        } while (vf::LengthSq(randVel) == 0.0f);
        m_allBoids.push_back(std::make_shared<Boid>(randPos, randVel, 10.0f));
    }

    AddGlobalAttractionPoint(InteractivePoint(sf::Vector2f(0.0f, 0.0f), 100.0f));
}

void FlockingController::Update(sf::Time const &dt)
{
    ComputeBoidsVisibleNeighbors();
    Flock();
    for (auto &boid : m_allBoids)
    {
        boid->SetSeeingDistance(m_seeingDistance);
        boid->SetSeeingAngle(m_seeingAngle);
        boid->SetSeparationStrength(m_separationStrength);
        boid->SetAlignmentStrength(m_alignmentStrength);
        boid->SetCohesionStrength(m_cohesionStrength);
        boid->SetSpeed(m_speed);
        boid->Update(dt);
    }
}

void FlockingController::Draw(Graphics &gfx)
{
    for (auto &boid : m_allBoids)
    {
        if (m_drawBoids)
            boid->Draw(gfx);
        if (m_drawNeighborLines)
            boid->DrawLineToNeighbors(gfx);
        if (m_drawVision)
            boid->DrawVisionShape(gfx);
    }
}

void FlockingController::Flock()
{
    for (auto &boid : m_allBoids)
    {
        sf::Vector2f force(0.0f, 0.0f);
        force += vf::Unit(boid->Separation()) * boid->GetSeparationStrength();
        force += vf::Unit(boid->Alignment()) * boid->GetAlignmentStrength();
        force += vf::Unit(boid->Cohesion()) * boid->GetCohesionStrength();
        for (auto &point : m_globalAttractionPoints)
        {
            force += vf::Unit(point.position - boid->GetPosition()) * point.strength;
        }
        for (auto &point : m_globalRepulsionPoints)
        {
            force += vf::Unit(boid->GetPosition() - point.position) * point.strength;
        }
        boid->SetAcceleration(force);
    }
}

void FlockingController::ComputeBoidsVisibleNeighbors()
{
    for (size_t i = 0; i < m_allBoids.size(); i++)
    {
        for (size_t j = 0; j < m_allBoids.size(); j++)
        {
            if (m_allBoids[i] != m_allBoids[j])
            {
                const sf::Vector2f _thisPos = m_allBoids[i]->GetPosition();
                const sf::Vector2f _checkPos = m_allBoids[j]->GetPosition();
                const sf::Vector2f dir = m_allBoids[i]->GetDirectionVector();
                const std::pair<sf::Vector2f, sf::Vector2f> visionBorders = m_allBoids[i]->GetVisionLimitBorders();

                const bool inSeeingDistance = vf::Distance(_thisPos, _checkPos) < m_allBoids[i]->GetSeeingDistance();
                const bool inPeripheral = !vf::isLeft(_thisPos, visionBorders.first, _checkPos) && vf::isLeft(_thisPos, visionBorders.second, _checkPos);
                const bool angleBiggerThanPI = m_allBoids[i]->GetSeeingAngle() > Math::Constants::PI;
                const bool angleBiggerThan2PI = m_allBoids[i]->GetSeeingAngle() > 2 * Math::Constants::PI;
                const bool isInFront = angleBiggerThanPI && vf::isLeft(vf::Perpendicular(dir), _thisPos, _checkPos);

                if (inSeeingDistance &&
                    (inPeripheral ||
                     (angleBiggerThanPI && isInFront) ||
                     (angleBiggerThan2PI)))
                {
                    m_allBoids[i]->AddNeighbor(m_allBoids[j]);
                }
                else
                {
                    m_allBoids[i]->RemoveNeighbor(m_allBoids[j]);
                }
            }
        }
    }
}
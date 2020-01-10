#include "FlockingController.hpp"

#include "Random.hpp"
#include "Camera.hpp"

FlockingController::FlockingController(int const &nBoids)
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
}

void FlockingController::Update(sf::Time const &dt)
{
    ComputeBoidsVisibleNeighbors();
    Flock();
    for (auto &boid : m_allBoids)
    {
        boid->Update(dt);
    }
}

void FlockingController::Draw(Graphics &gfx)
{
    for (auto &boid : m_allBoids)
    {
        boid->Draw(gfx);
        // boid->DrawLineToNeighbors(gfx);
        // boid->DrawCollideZoneCircle(gfx);
        boid->DrawVisionShape(gfx);
    }
}

void FlockingController::Flock()
{
    for (auto &boid : m_allBoids)
    {
        sf::Vector2f force(0.0f, 0.0f);
        force += boid->Separation() * boid->GetSeparationStrength();
        force += boid->Alignment() * boid->GetAlignmentStrength();
        force += boid->Cohesion() * boid->GetCohesionStrength();
        boid->SetAcceleration(force);
    }
}

void FlockingController::ComputeBoidsVisibleNeighbors()
{
    for (size_t i = 0; i < m_allBoids.size(); i++)
    {
        for (size_t j = 0; j < m_allBoids.size(); j++)
        {
            if (m_allBoids[i] != m_allBoids[j] &&
                vf::Distance(m_allBoids[i]->GetPosition(), m_allBoids[j]->GetPosition()) < m_allBoids[i]->GetSeeingDistance())
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
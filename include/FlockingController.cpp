#include "FlockingController.hpp"

#include "Random.hpp"
#include "Camera.hpp"
#include "Math.hpp"

FlockingController::FlockingController(InputUtility &iu, int const &nBoids)
    : m_iu(iu),
      m_simulationBox(-Camera::OffsetX, -Camera::OffsetY, 2 * Camera::OffsetX, 2 * Camera::OffsetY),
      m_seeingDistance(100.0f),
      m_seeingAngle(Math::Constants::PI / 2.0f),
      m_separationStrength(1.0f),
      m_alignmentStrength(1.0f),
      m_cohesionStrength(1.0f),
      m_speed(50.0f),
      m_drawBoids(true),
      m_drawVisibleNeighborLines(false),
      m_drawVision(false),
      m_drawFlocks(false),
      m_currentTimer(0.0f)
{
    for (int i = 0; i < nBoids; i++)
    {
        sf::Vector2f randPos(Random::Generate(-Camera::OffsetX, Camera::OffsetX), Random::Generate(-Camera::OffsetY, Camera::OffsetY));
        sf::Vector2f randVel;
        do
        {
            randVel = vf::Unit(sf::Vector2f(Random::Generate(-10.0f, 10.0f), Random::Generate(-10.0f, 10.0f))) * 50.0f;
        } while (vf::LengthSq(randVel) == 0.0f);
        m_allBoids.push_back(std::make_shared<Boid>(randPos, randVel, 10.0f));
    }
}

void FlockingController::Update(sf::Time const &dt)
{
    if (m_currentTimer > 1.0f / 3.0f)
    {
        ComputeBoidsNeighbors();
        ComputeBoidsFlockMates();
        m_currentTimer = 0.0f;
    }
    else
    {
        m_currentTimer += dt.asSeconds();
    }

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
        if (m_drawVisibleNeighborLines)
            boid->DrawLineToVisibleNeighbors(gfx);
        if (m_drawVision)
            boid->DrawVisionShape(gfx);
        if (m_drawFlocks)
        {
            for (auto flock : m_allFlocks)
            {
                std::vector<sf::Vector2f> flockPoints;
                for (auto &boid : flock)
                {
                    flockPoints.push_back(boid->GetPosition());
                }
                std::vector<sf::Vector2f> wrapped(Funclib::WrapPoints(&flockPoints));
                sf::Vector2f centroid(0, 0);
                int n = 0;
                for (auto &point : wrapped)
                {
                    centroid += point;
                    n++;
                }
                if (n > 0)
                {
                    centroid.x /= n;
                    centroid.y /= n;
                }
                for (auto &point : wrapped)
                {
                    point -= centroid;
                    point *= 1.2f;
                    point += centroid;
                }
                for (size_t i = 0; i < wrapped.size() - 1; i++)
                {
                    gfx.DrawLine(wrapped[i], wrapped[i + 1]);
                }
            }
        }
    }
}

void FlockingController::Flock()
{
    for (auto &boid : m_allBoids)
    {
        sf::Vector2f pos(boid->GetPosition());
        sf::Vector2f force(0.0f, 0.0f);
        force += boid->Separation() * boid->GetSeparationStrength();
        force += boid->Alignment() * boid->GetAlignmentStrength();
        force += boid->Cohesion() * boid->GetCohesionStrength();
        for (auto &point : m_globalAttractionPoints)
        {
            force += (point.position - pos) * point.strength;
        }
        for (auto &point : m_globalRepulsionPoints)
        {
            force.x += 1 / (pos - point.position).x * point.strength;
            force.y += 1 / (pos - point.position).y * point.strength;
        }

        if (!m_simulationBox.contains(pos))
        {
            force += sfmlext::GetCentroidOfRectangle(m_simulationBox) - pos;
        }

        if (!m_neutralMouseMode)
        {
            sf::Vector2f mouseEffect(0.0f, 0.0f);
            if (m_attractMouseMode)
            {
                mouseEffect += (sf::Vector2f)m_iu.GetMousePosition() - pos;
            }
            else if (m_repelMouseMode)
            {
                mouseEffect += pos - (sf::Vector2f)m_iu.GetMousePosition();
            }
            float length = vf::Length(mouseEffect);
            float strength = 500.0f;
            mouseEffect *= (strength / powf(length, 1.3f));
            force += mouseEffect;
        }
        boid->SetAcceleration(force);
    }
}

void FlockingController::ComputeBoidsNeighbors()
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

                if (inSeeingDistance)
                {
                    m_allBoids[i]->AddNeighbor(m_allBoids[j]);
                }
                else
                {
                    m_allBoids[i]->RemoveNeighbor(m_allBoids[j]);
                }

                if (inSeeingDistance &&
                    (inPeripheral ||
                     (angleBiggerThanPI && isInFront) ||
                     (angleBiggerThan2PI)))
                {
                    m_allBoids[i]->AddVisibleNeighbor(m_allBoids[j]);
                }
                else
                {
                    m_allBoids[i]->RemoveVisibleNeighbor(m_allBoids[j]);
                }
            }
        }
    }
}

void FlockingController::ComputeBoidsFlockMates()
{
    m_allFlocks.clear();
    for (auto boid : m_allBoids)
    {
        boid->SetInFlock(false);
    }
    for (auto &boid : m_allBoids)
    {
        if (!boid->GetInFlock())
        {
            std::set<std::shared_ptr<Boid>> currentFlock;
            IterativeFlockCheck(boid, currentFlock);
            m_allFlocks.push_back(currentFlock);
        }
    }
}

void FlockingController::IterativeFlockCheck(std::shared_ptr<Boid> const &boid, std::set<std::shared_ptr<Boid>> &currentFlock)
{
    for (auto &neighbor : boid->GetNeighbors())
    {
        if (currentFlock.emplace(neighbor).second)
        {
            neighbor->SetInFlock(true);
            IterativeFlockCheck(neighbor, currentFlock);
        }
    }
}
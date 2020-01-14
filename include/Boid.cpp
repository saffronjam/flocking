#include "Boid.hpp"

#include "Funclib.hpp"
#include "Math.hpp"

Boid::Boid(sf::Vector2f position, sf::Vector2f startVelocity, float collideZoneRadius)
    : m_position(position),
      m_velocity(startVelocity),
      m_acceleration(0.0f, 0.0f),
      m_directionVector(vf::Unit(m_velocity)),
      m_visionShapeMaxPoints(70),
      m_visionShape(sf::TrianglesFan, m_visionShapeMaxPoints),
      m_collideZoneRadius(collideZoneRadius),
      m_seeingDistance(100.0f),
      m_seeingAngle(Math::Constants::PI / 2.0f),
      m_separationStrength(2.0f),
      m_alignmentStrength(2.0f),
      m_cohesionStrength(2.0f),
      m_speed(50.0f),
      m_inFlock(false)
{
    m_body.setPointCount(3);
    m_body.setFillColor(sf::Color(230, 0, 0, 110));
    m_body.setOutlineColor(sf::Color(100, 100, 100, 200));
    m_body.setOutlineThickness(1.0f);
}

void Boid::Update(sf::Time const &dt)
{
    CorrectBodyAccordingToDirection();
    CorrectVisionAccordingToDirection();

    m_velocity += gf::Constrain(m_acceleration * dt.asSeconds(), 0.0f, 10.0f);
    UpdateDirectionVector();
    m_velocity = gf::Constrain(m_velocity, 0.0f, m_speed);
    m_position += m_velocity * dt.asSeconds();
}

void Boid::Draw(Graphics &gfx)
{
    gfx.Draw(m_body);
}

void Boid::DrawLineToVisibleNeighbors(Graphics &gfx)
{
    for (auto &neighbor : m_visibleNeighbors)
    {
        gfx.DrawLine(m_position, neighbor->GetPosition(), sf::Color(150, 150, 150, 100));
    }
}

void Boid::DrawVisionShape(Graphics &gfx)
{
    gfx.Draw(m_visionShape);
}

sf::Vector2f Boid::Separation()
{
    sf::Vector2f average(0.0f, 0.0f);
    size_t n = 0;
    for (auto &neighbor : GetVisibleNeighbors())
    {
        n++;
        average.x += (GetPosition() - neighbor->GetPosition()).x;
        average.y += (GetPosition() - neighbor->GetPosition()).y;
    }
    if (n > 0)
    {
        average.x /= GetVisibleNeighbors().size();
        average.y /= GetVisibleNeighbors().size();
    }
    return average * 1.5f;
}

sf::Vector2f Boid::Alignment()
{
    sf::Vector2f average(0.0f, 0.0f);
    size_t n = 0;
    for (auto &neighbor : GetVisibleNeighbors())
    {
        n++;
        average.x += neighbor->GetVelocity().x;
        average.y += neighbor->GetVelocity().y;
    }
    if (n > 0)
    {
        average.x /= GetVisibleNeighbors().size();
        average.y /= GetVisibleNeighbors().size();
    }

    return average * 1.5f;
}

sf::Vector2f Boid::Cohesion()
{
    sf::Vector2f average(0.0f, 0.0f);
    sf::Vector2f steer;
    size_t n = 0;
    for (auto &neighbor : GetVisibleNeighbors())
    {
        n++;
        average.x += neighbor->GetPosition().x;
        average.y += neighbor->GetPosition().y;
    }
    if (n > 0)
    {
        average.x /= GetVisibleNeighbors().size();
        average.y /= GetVisibleNeighbors().size();
        steer = average - GetPosition();
    }
    else
    {
        steer = average;
    }

    return steer;
}

std::pair<sf::Vector2f, sf::Vector2f> Boid::GetVisionLimitBorders() const
{
    return std::pair<sf::Vector2f, sf::Vector2f>(m_visionShape[1].position, m_visionShape[m_visionShapeMaxPoints - 1].position);
}

void Boid::UpdateDirectionVector()
{
    if (vf::Length(m_velocity) > 0.0f)
    {
        m_directionVector = vf::Unit(m_velocity);
    }
}

void Boid::CorrectBodyAccordingToDirection()
{
    const sf::Vector2f frontPoint(m_position + m_directionVector * m_collideZoneRadius);
    const sf::Vector2f backLeftPoint(vf::Rotate(m_position - m_directionVector * m_collideZoneRadius, -Math::Constants::PI / 4.0f, m_position));
    const sf::Vector2f backRightPoint(vf::Rotate(m_position - m_directionVector * m_collideZoneRadius, Math::Constants::PI / 4.0f, m_position));

    m_body.setPoint(0, frontPoint);
    m_body.setPoint(1, backLeftPoint);
    m_body.setPoint(2, backRightPoint);
}

void Boid::CorrectVisionAccordingToDirection()
{
    m_visionShape[0] = m_position;
    m_visionShape[0].color = sf::Color(sf::Color(200, 200, 200, 150));
    float angle = 0.0f;
    int i = 1;
    for (; i < m_visionShapeMaxPoints && angle < m_seeingAngle; i++, angle += m_seeingAngle / m_visionShapeMaxPoints)
    {
        m_visionShape[i] = vf::Rotate(m_position + m_directionVector * m_seeingDistance, angle - m_seeingAngle / 2.0f, m_position);
        m_visionShape[i].color = sf::Color(sf::Color(70, 40, 40, 30));
    }
}

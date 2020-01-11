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
      m_speed(50.0f)
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

    m_velocity += m_acceleration * dt.asSeconds();
    UpdateDirectionVector();
    m_velocity = m_directionVector * m_speed;
    m_position += m_velocity * dt.asSeconds();
}

void Boid::Draw(Graphics &gfx)
{
    gfx.Draw(m_body);
}

void Boid::DrawLineToNeighbors(Graphics &gfx)
{
    for (auto &neighbor : m_neighbors)
    {
        gfx.DrawLine(m_position, neighbor->GetPosition());
    }
}

void Boid::DrawVisionShape(Graphics &gfx)
{
    gfx.Draw(m_visionShape);
}

sf::Vector2f Boid::Separation()
{
    sf::Vector2f average;
    for (auto &neighbor : GetNeighbors())
    {
        average.x += (GetPosition() - neighbor->GetPosition()).x;
        average.y += (GetPosition() - neighbor->GetPosition()).y;
    }
    average.x /= GetNeighbors().size();
    average.y /= GetNeighbors().size();

    return average;
}

sf::Vector2f Boid::Alignment()
{
    sf::Vector2f average;
    for (auto &neighbor : GetNeighbors())
    {
        average.x += neighbor->GetVelocity().x;
        average.y += neighbor->GetVelocity().y;
    }
    average.x /= GetNeighbors().size();
    average.y /= GetNeighbors().size();

    return average;
}

sf::Vector2f Boid::Cohesion()
{
    sf::Vector2f average;
    for (auto &neighbor : GetNeighbors())
    {
        average.x += neighbor->GetPosition().x;
        average.y += neighbor->GetPosition().y;
    }
    average.x /= GetNeighbors().size();
    average.y /= GetNeighbors().size();

    sf::Vector2f steer = average - GetPosition();

    return steer;
}

void Boid::Clamp(sf::FloatRect const &box)
{
    if (m_position.x < box.left)
    {
        m_position.x = box.left + box.width;
    }
    else if (m_position.x > box.left + box.width)
    {
        m_position.x = box.left;
    }

    if (m_position.y < box.top)
    {
        m_position.y = box.top + box.height;
    }
    else if (m_position.y > box.top + box.height)
    {
        m_position.y = box.top;
    }
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
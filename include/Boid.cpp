#include "Boid.hpp"

#include "Funclib.hpp"
#include "Math.hpp"

Boid::Boid(sf::Vector2f position, sf::Vector2f startVelocity, float collideZoneRadius)
    : m_position(position),
      m_velocity(startVelocity),
      m_acceleration(0.0f, 0.0f),
      m_collideZoneRadius(collideZoneRadius),
      m_seeingDistance(100.0f),
      m_seeingAngle(Math::Constants::PI / 2.0f),
      m_separationStrength(1.0f),
      m_alignmentStrength(1.0f),
      m_cohesionStrength(1.0f)
{
    m_body.setPointCount(3);
    m_body.setFillColor(sf::Color::Red);
    m_body.setOutlineColor(sf::Color::White);

    m_collideZoneRadiusShape.setRadius(m_collideZoneRadius);
    m_collideZoneRadiusShape.setFillColor(sf::Color::Transparent);
    m_collideZoneRadiusShape.setOutlineThickness(1.0f);
    m_collideZoneRadiusShape.setOutlineColor(sf::Color::White);
}

#include "Random.hpp"
void Boid::Update(sf::Time const &dt)
{
    CorrectBodyAccordingToDirection();
    m_acceleration = gf::Constrain(m_acceleration, -5.0f, 5.0f);
    m_velocity += m_acceleration * dt.asSeconds();
    m_velocity = gf::Constrain(m_velocity, -50.0f, 50.0f);
    m_velocity = gf::Constrain(m_velocity, -50.0f, 50.0f);
    // m_velocity = gf::ConstrainLower(m_velocity, 1.0f);
    // m_velocity = gf::ConstrainUpper(m_velocity, 1.0f);
    m_position += m_velocity * dt.asSeconds();
    m_collideZoneRadiusShape.setPosition(m_position - sf::Vector2f(m_collideZoneRadius, m_collideZoneRadius));
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

void Boid::DrawCollideZoneCircle(Graphics &gfx)
{
    gfx.Draw(m_collideZoneRadiusShape);
}

void Boid::DrawVisionShape(Graphics &gfx)
{
    const sf::Vector2f forwardLeft(vf::Rotate(m_position + vf::Unit(m_velocity) * m_seeingDistance, -m_seeingAngle / 2.0f, m_position));
    const sf::Vector2f forwardRight(vf::Rotate(m_position + vf::Unit(m_velocity) * m_seeingDistance, m_seeingAngle / 2.0f, m_position));

    gfx.DrawLine(m_position, forwardLeft);
    gfx.DrawLine(m_position, forwardRight);
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

void Boid::CorrectBodyAccordingToDirection()
{
    const sf::Vector2f frontPoint(m_position + vf::Unit(m_velocity) * m_collideZoneRadius);
    const sf::Vector2f backLeftPoint(vf::Rotate(m_position - vf::Unit(m_velocity) * m_collideZoneRadius, -Math::Constants::PI / 4.0f, m_position));
    const sf::Vector2f backRightPoint(vf::Rotate(m_position - vf::Unit(m_velocity) * m_collideZoneRadius, Math::Constants::PI / 4.0f, m_position));

    m_body.setPoint(0, frontPoint);
    m_body.setPoint(1, backLeftPoint);
    m_body.setPoint(2, backRightPoint);
}

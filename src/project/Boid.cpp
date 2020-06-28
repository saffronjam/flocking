#include "Boid.h"

Boid::Boid(const sf::Vector2f &position)
    : m_bodyColor(sf::Color::Red),
      m_sightRadius(100.0f),
      m_sightAngle(100.0f)
{
    m_voidBody = VoidBody::Create(position);
}

void Boid::DrawBody() const
{
    sf::Vector2f front = GetForward() * 8.0f;
    sf::Vector2f backLeft = vl::Rotate(-front, Lib::ToRadians(-30.0f), vl::Null<>());
    sf::Vector2f backRight = vl::Rotate(-front, Lib::ToRadians(30.0f), vl::Null<>());

    sf::Vector2f position = GetPosition();
    sf::VertexArray triangle(sf::PrimitiveType::Triangles, 3);
    triangle[0].position = front + position;
    triangle[1].position = backLeft + position;
    triangle[2].position = backRight + position;

    triangle[0].color = m_bodyColor;
    triangle[1].color = m_bodyColor;
    triangle[2].color = m_bodyColor;

    Camera::Draw(triangle);
}

void Boid::DrawSight() const
{
    sf::VertexArray sightArea(sf::PrimitiveType::TriangleFan, 1 + m_sightAngle);
    sightArea[0] = sf::Vertex(GetPosition(), sf::Color(200, 200, 200, 150));
    sf::Vector2f position = GetPosition();
    sf::Vector2f leftStart = vl::Rotate(GetForward(), Lib::ToRadians(-m_sightAngle / 2.0f), vl::Null<>()) * m_sightRadius + position;
    for (int i = 1; i < static_cast<int>(m_sightAngle) + 1; i++)
    {
        sightArea[i] = sf::Vertex(vl::Rotate(leftStart, Lib::ToRadians(static_cast<float>(i)), position), sf::Color(40, 40, 40, 30));
    }
    Camera::Draw(sightArea);
}

void Boid::DrawNeighbors() const
{
    for (auto &neighbor : m_neighbors)
    {
        Camera::DrawLine(GetPosition(), neighbor->GetPosition());
    }
}

void Boid::DrawVisibleNeighbors() const
{
    for (auto &neighbor : m_visibleNeighbors)
    {
        Camera::DrawLine(GetPosition(), neighbor->GetPosition(), sf::Color::Blue);
    }
}

void Boid::DrawVelocity() const
{
    Camera::DrawLine(GetPosition(), GetPosition() + vl::ConvertTo<sf::Vector2f>(m_voidBody->GetBody()->GetLinearVelocity()) * 5.0f, sf::Color::Green);
}

void Boid::DrawAcceleration() const
{
}

void Boid::ApplyForce(const sf::Vector2f &force) noexcept
{
    m_voidBody->GetBody()->ApplyForceToCenter(vl::ConvertTo<b2Vec2>(force), true);
}

sf::Vector2f Boid::GetForward() const noexcept
{
    return vl::Unit(vl::ConvertTo<sf::Vector2f>(m_voidBody->GetBody()->GetLinearVelocity()));
}

std::pair<sf::Vector2f, sf::Vector2f> Boid::GetSightBounds() const
{
    sf::Vector2f position = GetPosition();
    sf::Vector2f forward = GetForward();
    return std::make_pair(vl::Rotate(forward, Lib::ToRadians(-m_sightAngle / 2.0f), vl::Null<>()) * m_sightRadius + position,
                          vl::Rotate(forward, Lib::ToRadians(m_sightAngle / 2.0f), vl::Null<>()) * m_sightRadius + position);
}
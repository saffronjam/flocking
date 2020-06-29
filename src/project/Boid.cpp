#include "Boid.h"

Boid::Boid(const sf::Vector2f &position)
    : m_position(position),
      m_velocity(0.0f, 0.0f),
      m_acceleration(0.0f, 0.0f),
      m_minSpeed(1.0f),
      m_maxSpeed(150.0f),
      m_separationMultiplier(1.0f),
      m_alignmentMultiplier(1.0f),
      m_cohesionMultiplier(1.0f),
      m_forward(1.0f, 0.0f),
      m_bodyColor(sf::Color::Red),
      m_sightRadius(150.0f),
      m_sightAngle(365.0f),
      m_visionShape(sf::PrimitiveType::TriangleFan, 361)
{
    ReconstructVisionShape();
    sf::Color color = Random::Color();
    for (size_t i = 0; i < m_visionShape.getVertexCount(); i++)
    {
        m_visionShape[i].color = color;
        m_visionShape[i].color.a = 5.0f;
    }
}

void Boid::Update()
{
    m_velocity += Lib::Constrain(m_acceleration * Clock::Delta().asSeconds(), 0.0f, 10.0f);
    m_velocity = Lib::Constrain(m_velocity, m_minSpeed, m_maxSpeed);
    m_position += m_velocity * Clock::Delta().asSeconds();
    m_acceleration = vl::Null<>();
    m_forward = vl::Unit(GetVelocity());

    ReconstructVisionShape();
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
    Camera::Draw(m_visionShape);
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
    Camera::DrawLine(GetPosition(), GetPosition() + GetVelocity(), sf::Color::Green);
}

void Boid::DrawAcceleration() const
{
    Camera::DrawLine(GetPosition(), GetPosition() + GetAcceleration(), sf::Color::Blue);
}

sf::Vector2f Boid::GetSeparationForce() const
{
    sf::Vector2f average(0.0f, 0.0f);
    const size_t numNeighbors = GetVisibleNeighbors().size();
    for (auto &neighbor : GetVisibleNeighbors())
    {
        average += GetPosition() - neighbor->GetPosition();
    }
    if (numNeighbors > 0)
    {
        average /= static_cast<float>(numNeighbors);
    }
    return average * 5.0f;
}

sf::Vector2f Boid::GetAlignmentForce() const
{
    sf::Vector2f average(0.0f, 0.0f);
    const size_t numNeighbors = GetVisibleNeighbors().size();
    for (auto &neighbor : GetVisibleNeighbors())
    {
        average += neighbor->GetVelocity();
    }
    if (numNeighbors > 0)
    {
        average /= static_cast<float>(numNeighbors);
    }
    return average * 10.0f;
}

sf::Vector2f Boid::GetCohesionForce() const
{
    sf::Vector2f average(0.0f, 0.0f);
    sf::Vector2f steer(0.0f, 0.0f);
    const size_t numNeighbors = GetVisibleNeighbors().size();
    for (auto &neighbor : GetVisibleNeighbors())
    {
        average += neighbor->GetPosition();
    }
    if (numNeighbors > 0)
    {
        average /= static_cast<float>(numNeighbors);
        steer = average - GetPosition();
    }
    return steer;
}

void Boid::ApplyForce(const sf::Vector2f &force) noexcept
{
    m_acceleration += force;
}

sf::Vector2f Boid::GetForward() const noexcept
{
    return m_forward;
}

std::pair<sf::Vector2f, sf::Vector2f> Boid::GetSightBounds() const
{
    sf::Vector2f position = GetPosition();
    sf::Vector2f forward = GetForward();
    return std::make_pair(vl::Rotate(forward, Lib::ToRadians(-m_sightAngle / 2.0f), vl::Null<>()) * m_sightRadius + position,
                          vl::Rotate(forward, Lib::ToRadians(m_sightAngle / 2.0f), vl::Null<>()) * m_sightRadius + position);
}

void Boid::SetSightRadius(float radius) noexcept
{
    m_sightRadius = radius;
}

void Boid::SetSightAngle(float angle) noexcept
{
    m_sightAngle = angle;
}

void Boid::ReconstructVisionShape() noexcept
{
    m_visionShape[0].position = GetPosition();
    sf::Vector2f leftStart = vl::Rotate(GetForward(), Lib::ToRadians(-GetSightAngle() / 2.0f), vl::Null<>()) * GetSightRadius() + GetPosition();
    for (int i = 1; i < 361; i++)
    {
        float angle = Lib::Constrain(GetSightAngle() / 360.0f * static_cast<float>(i - 1), 0.0f, 360.0f);
        m_visionShape[i].position = vl::Rotate(leftStart, Lib::ToRadians(angle), GetPosition());
    }
}
#include "Boid.h"

namespace Se
{
Boid::Boid(const sf::Vector2f &position) :
	_position(position),
	_velocity(0.0f, 0.0f),
	_acceleration(0.0f, 0.0f),
	_minSpeed(1.0f),
	_maxSpeed(100.0f),
	_maxForce(300.0f),
	_separationMultiplier(1.0f),
	_alignmentMultiplier(1.0f),
	_cohesionMultiplier(1.0f),
	_sightRadius(150.0f),
	_sightAngle(365.0f),
	_forward(1.0f, 0.0f),
	_bodyShape(3),
	_visionShape(sf::PrimitiveType::TriangleFan, 361)
{
	ComputeVisionShape();
	for ( size_t i = 0; i < _visionShape.getVertexCount(); i++ )
	{
		_visionShape[i].color = _visionShapeColor;
		_visionShape[i].color.a = 5.0f;
	}

	const sf::Vector2f front = GetForward() * 12.0f;
	const sf::Vector2f backLeft = VecUtils::Rotate(-front, GenUtils::ToRadians(-30.0f), VecUtils::Null<>());
	const sf::Vector2f backRight = VecUtils::Rotate(-front, GenUtils::ToRadians(30.0f), VecUtils::Null<>());

	_bodyShape.setPoint(0, front);
	_bodyShape.setPoint(1, backLeft);
	_bodyShape.setPoint(2, backRight);

	_bodyShape.setFillColor(sf::Color(255, 0, 0, 150));
	_bodyShape.setOutlineThickness(1.0f);
	_bodyShape.setOutlineColor(sf::Color(50, 50, 50, 200));

	_visionShapeColor.a = 40.0f;
}

void Boid::Update()
{
	const auto dt = Global::Clock::GetFrameTime();

	_velocity += _acceleration * dt.asSeconds();
	_velocity = VecUtils::Constrain(_velocity, _minSpeed, _maxSpeed);
	_position += _velocity * dt.asSeconds();
	_forward = VecUtils::Unit(GetVelocity());

	_acceleration *= 0.9f;

	_bodyShape.setPosition(_position);
	GenUtils::Rotate(_bodyShape, _forward);
}

void Boid::RenderVision(Scene &scene) const
{
	scene.Submit(_visionShape);
}

sf::Vector2f Boid::GetSeparationForce() const
{
	sf::Vector2f average(0.0f, 0.0f);
	const size_t numNeighbors = GetVisibleNeighbors().size();
	for ( const auto &neighbor : GetVisibleNeighbors() )
	{
		auto diff = GetPosition() - neighbor->GetPosition();
		if ( VecUtils::LengthSq(diff) != 0.0f )
		{
			average += diff / _sightRadius;
		}
		else
		{
			average += sf::Vector2f(100.0f, 0.0f);
		}
	}
	if ( numNeighbors > 0 )
	{
		average /= static_cast<float>(numNeighbors);
		average = VecUtils::SetLength(average, _maxForce);
	}
	return average;
}

sf::Vector2f Boid::GetAlignmentForce() const
{
	sf::Vector2f average(0.0f, 0.0f);
	const size_t numNeighbors = GetVisibleNeighbors().size();
	for ( const auto &neighbor : GetVisibleNeighbors() )
	{
		average += neighbor->GetVelocity();
	}
	if ( numNeighbors > 0 )
	{
		average /= static_cast<float>(numNeighbors);
		average = VecUtils::SetLength(average, _maxForce);
	}
	return average;
}

sf::Vector2f Boid::GetCohesionForce() const
{
	sf::Vector2f average(0.0f, 0.0f);
	sf::Vector2f steer(0.0f, 0.0f);
	const size_t numNeighbors = GetVisibleNeighbors().size();
	for ( const auto &neighbor : GetVisibleNeighbors() )
	{
		average += neighbor->GetPosition();
	}
	if ( numNeighbors > 0 )
	{
		average /= static_cast<float>(numNeighbors);
		average = VecUtils::SetLength(average - GetPosition(), _maxForce);
		steer = average;
	}
	return steer;
}

void Boid::ApplyForce(const sf::Vector2f &force)
{
	_acceleration += force;
}

sf::Vector2f Boid::GetForward() const
{
	return _forward;
}

Pair<sf::Vector2f, sf::Vector2f> Boid::GetSightBounds() const
{
	const sf::Vector2f position = GetPosition();
	const sf::Vector2f forward = GetForward();
	return CreatePair(VecUtils::Rotate(forward, GenUtils::ToRadians(-_sightAngle / 2.0f), VecUtils::Null<>()) * _sightRadius + position,
					  VecUtils::Rotate(forward, GenUtils::ToRadians(_sightAngle / 2.0f), VecUtils::Null<>()) * _sightRadius + position);
}

void Boid::SetSightRadius(float radius)
{
	_sightRadius = radius;
}

void Boid::SetSightAngle(float angle)
{
	_sightAngle = angle;
}

void Boid::ComputeVisionShape()
{
	_visionShape[0].position = GetPosition();
	const sf::Vector2f leftStart = VecUtils::Rotate(GetForward(), GenUtils::ToRadians(-GetSightAngle() / 2.0f), VecUtils::Null<>()) * GetSightRadius() + GetPosition();
	for ( int i = 1; i < 361; i++ )
	{
		float angle = GenUtils::Constrain(GetSightAngle() / 360.0f * static_cast<float>(i - 1), 0.0f, 360.0f);
		_visionShape[i].position = VecUtils::Rotate(leftStart, GenUtils::ToRadians(angle), GetPosition());
	}
}
}

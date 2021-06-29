#pragma once

#include <Saffron.h>

namespace Se
{
class Boid
{
public:
	explicit Boid(const sf::Vector2f &position);

	bool operator==(const Boid &otherBoid) const { return _uuid == otherBoid._uuid; }
	bool operator<(const Boid &otherBoid) const { return _uuid < otherBoid._uuid; }

	void Update();
	void RenderVision(Scene &scene) const;

	sf::Vector2f GetSeparationForce() const;
	sf::Vector2f GetAlignmentForce() const;
	sf::Vector2f GetCohesionForce() const;

	void AddNeighbor(const Boid *boid) { _neighbors.emplace(boid); }
	void AddVisibleNeighbor(const Boid *boid) { _visibleNeighbors.emplace(boid); }
	void ClearNeighbors() { _neighbors.clear(); }
	void ClearVisibleNeighbors() { _visibleNeighbors.clear(); }
	const TreeSet<const Boid *> &GetNeighbors() const { return _neighbors; }
	const TreeSet<const Boid *> &GetVisibleNeighbors() const { return _visibleNeighbors; }

	void ApplyForce(const sf::Vector2f &force);
	void ResetForce() { _acceleration = VecUtils::Null<>(); }

	const sf::Vector2f &GetPosition() const { return _position; }
	void SetPosition(const sf::Vector2f &position) { _position = position; }

	float GetSightRadius() const { return _sightRadius; }
	float GetSightAngle() const { return _sightAngle; }
	Pair<sf::Vector2f, sf::Vector2f> GetSightBounds() const;

	sf::Vector2f GetForward() const;
	sf::Vector2f GetVelocity() const { return _velocity; };
	sf::Vector2f GetAcceleration() const { return _acceleration; };

	float GetMaxSpeed() const { return _maxSpeed; }
	void SetMinSpeed(float speed) { _minSpeed = speed; }
	void SetMaxSpeed(float speed) { _maxSpeed = speed; }

	float GetSeparationMultiplier() const { return _separationMultiplier; }
	float GetAlignmentMultiplier() const { return _alignmentMultiplier; }
	float GetCohesionMultiplier() const { return _cohesionMultiplier; }

	const sf::ConvexShape &GetBodyShape() const { return _bodyShape; }
	sf::FloatRect GetBoundingBox() const { return _bodyShape.getLocalBounds(); }

	sf::Color GetBodyColor() const { return _bodyColor; }
	sf::Color GetVisionShapeColor() const { return _visionShapeColor; }
	sf::Color GetNeighborLinesColor() const { return _neighborLinesColor; }
	sf::Color GetVisibleNeighborLinesColor() const { return _visibleNeighborLinesColor; }
	sf::Color GetVelocityLineColor() const { return _velocityLineColor; }
	sf::Color GetAccelerationLineColor() const { return _accelerationLineColor; }

	void SetSightRadius(float radius);
	void SetSightAngle(float angle);

	void SetSeparationMultiplier(float multiplier) { _separationMultiplier = multiplier; }
	void SetAlignmentMultiplier(float multiplier) { _alignmentMultiplier = multiplier; }
	void SetCohesionMultiplier(float multiplier) { _cohesionMultiplier = multiplier; }

	void ComputeVisionShape();

private:
	UUID _uuid;

	sf::Vector2f _position;
	sf::Vector2f _velocity;
	sf::Vector2f _acceleration;

	float _minSpeed;
	float _maxSpeed;
	float _maxForce;

	float _separationMultiplier;
	float _alignmentMultiplier;
	float _cohesionMultiplier;

	TreeSet<const Boid *> _neighbors;
	TreeSet<const Boid *> _visibleNeighbors;

	sf::Color _bodyColor{ sf::Color::Red };
	sf::Color _visionShapeColor{ Random::Color() };
	sf::Color _neighborLinesColor{ 0, 0, 150, 70 };
	sf::Color _visibleNeighborLinesColor{ 120, 0, 255, 200 };
	sf::Color _velocityLineColor{ 0, 255, 0, 150 };
	sf::Color _accelerationLineColor{ 255, 0, 0, 150 };

	float _sightRadius;
	float _sightAngle;

	sf::Vector2f _forward;

	sf::ConvexShape _bodyShape;
	sf::VertexArray _visionShape;
};
}
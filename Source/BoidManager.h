#pragma once

#include <Saffron.h>

#include "Boid.h"

namespace Se
{
class BoidManager
{
public:
	explicit BoidManager(Camera &camera);

	void OnUpdate();
	void OnRender(Scene &scene);
	void OnGuiRender();
	void OnFirstRenderTargetResize(const sf::Vector2f &size);

	void Pause();
	void Resume();
	bool IsPaused() const { return _paused; }

	void SetBoidCount(int count);

	void SetSeparationMultiplier(float multiplier);
	void SetAlignmentMultiplier(float multiplier);
	void SetCohesionMultiplier(float multiplier);

	void SetMinSpeed(float speed);
	void SetMaxSpeed(float speed);

	void SetVisionRadius(float radius);
	void SetVisionAngle(float angle);

	void SetDrawBody(bool onoff) { _renderBodies = onoff; }
	void SetDrawVision(bool onoff) { _renderVision = onoff; }
	void SetDrawNeighbors(bool onoff) { _renderNeighbors = onoff; }
	void SetDrawVelocity(bool onoff) { _renderVelocities = onoff; }
	void SetDrawAcceleration(bool onoff) { _renderAccelerations = onoff; }
	void SetDrawFlocks(bool onoff) { _renderFlocks = onoff; }
	void SetDrawQuadtree(bool onoff) { _renderQuadtree = onoff; }

private:
	void CheckRenderState();
	void RenderBodies(Scene &scene) const;
	void RenderSightShapes(Scene &scene) const;
	void RenderNeighbors(Scene &scene) const;
	void RenderVisibleNeighbors(Scene &scene) const;
	void RenderVelocities(Scene &scene) const;
	void RenderAccelerations(Scene &scene) const;
	void RenderQuadTree(Scene &scene);
	void RenderFlockConvexHull(Scene &scene);

	void ComputeAllVisibleNeighbors();
	void ComputeFlocks();
	void IterativeFlockCheck(const Boid &boid, TreeSet<Boid *> &currentFlock);
	sf::Vector2f GetRepulsionBorderForce(const Boid &boid) const;

	void ComputeQuadTree();
	void ClearQuadtree();

	void ComputeNeighborLinesVA();
	void ComputeVisibleNeighborLinesVA();
	void ComputeVelocityLinesVA();
	void ComputeAccelerationLinesVA();
	void ComputeFlockLinesVA();
	void ComputeActiveQuadTreeGridVA();

	static void ComputeNeighborsLinesVAHelper(sf::VertexArray &va, const List<Boid> &boids, bool onlyVisible);
	static void ComputePhysicsLinesVAHelper(sf::VertexArray &va, const List<Boid> &boids, bool velocity);

private:
	Camera &_camera;

	sf::Vector2f _gridOffset = { 1000.0f, 600.0f };

	List<Boid> _boids;
	List<TreeSet<Boid *>> _flocks;
	TreeSet<Boid> _inFlock;
	List<List<List<Boid *>>> _quadtree;

	sf::VertexArray _neighborLinesVA{ sf::PrimitiveType::Lines };
	bool _wantComputeNeighborLines = true;

	sf::VertexArray _visibleNeighborLinesVA{ sf::PrimitiveType::Lines };
	bool _wantComputeVisibleNeighborLines = true;

	sf::VertexArray _velocityLinesVA{ sf::PrimitiveType::Lines };
	bool _wantComputeVelocityLines = true;

	sf::VertexArray _accelerationLinesVA{ sf::PrimitiveType::Lines };
	bool _wantComputeAccelerationLines = true;

	sf::VertexArray _flockLinesVA{ sf::PrimitiveType::Lines };
	bool _wantComputeFlockLines = true;

	sf::FloatRect _quadtreeRect;
	sf::FloatRect _quadtreeBox;
	TreeSet<Pair<int, int>> _activeQuadtreeContainers;
	sf::VertexArray _quadtreeGridVA{ sf::PrimitiveType::Lines };
	sf::VertexArray _activeQuadtreeGridVA{ sf::PrimitiveType::Quads };
	bool _wantComputeActiveQuadtreeGrid = true;

	sf::FloatRect _repulsionBorders;

	sf::Color _activeQuadtreeContainerColor{ 150, 0,0, 50 };
	sf::Color _flockLinesColor{ 90, 90,90, 50 };

	bool _paused;

	bool _renderBodies;
	bool _renderVision;
	bool _renderNeighbors;
	bool _renderVisibleNeighbors;
	bool _renderVelocities;
	bool _renderAccelerations;
	bool _renderFlocks;
	bool _renderQuadtree;

	// Gui cache
	int _noBoids = 100;
	float _separationMultiplier = 1.0f;
	float _alignmentMultiplier = 1.0f;
	float _cohesionMultiplier = 1.0f;
	float _minSpeed = 10.0f;
	float _maxSpeed = 100.0f;
	float _visionRadius = 150.0f;
	float _visionAngle = 360.0f;
};
}

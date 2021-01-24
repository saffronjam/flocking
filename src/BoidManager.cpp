#include "BoidManager.h"

#include <SFML/Graphics.hpp>

namespace Se
{
BoidManager::BoidManager(Camera &camera) :
	_camera(camera),
	_quadtreeRect(-_gridOffset, _gridOffset * 2.0f),
	_quadtreeBox(VecUtils::Null<>(), sf::Vector2f(_quadtreeRect.width, _quadtreeRect.width) / 50.0f),
	_repulsionBorders(_quadtreeRect),
	_paused(false),
	_renderBodies(true),
	_renderVision(false),
	_renderNeighbors(false),
	_renderVisibleNeighbors(false),
	_renderVelocities(false),
	_renderAccelerations(false),
	_renderFlocks(false),
	_renderQuadtree(false)
{
	// Setup quadtree
	_quadtree.resize(_quadtreeRect.width / _quadtreeBox.width);
	for ( auto &col : _quadtree )
	{
		col.resize(_quadtreeRect.height / _quadtreeBox.height);
	}

	for ( int i = 0; i <= _quadtree.size(); i++ )
	{
		_quadtreeGridVA.append(sf::Vertex(sf::Vector2f(_quadtreeRect.left + i * _quadtreeBox.width, _quadtreeRect.top), sf::Color(50, 50, 50, 15)));
		_quadtreeGridVA.append(sf::Vertex(sf::Vector2f(_quadtreeRect.left + i * _quadtreeBox.width, _quadtreeRect.top + _quadtreeRect.height), sf::Color(50, 50, 50, 15)));
	}
	for ( int j = 0; j <= _quadtree[0].size(); j++ )
	{
		_quadtreeGridVA.append(sf::Vertex(sf::Vector2f(_quadtreeRect.left, _quadtreeRect.top + j * _quadtreeBox.height), sf::Color(50, 50, 50, 15)));
		_quadtreeGridVA.append(sf::Vertex(sf::Vector2f(_quadtreeRect.left + _quadtreeRect.width, _quadtreeRect.top + j * _quadtreeBox.height), sf::Color(50, 50, 50, 15)));
	}

	SetSeparationMultiplier(_separationMultiplier);
	SetAlignmentMultiplier(_alignmentMultiplier);
	SetCohesionMultiplier(_cohesionMultiplier);
}

void BoidManager::OnUpdate()
{
	if ( !_paused )
	{
		ClearQuadtree();
		ComputeQuadTree();

		for ( auto &boid : _boids )
		{
			boid.ApplyForce(boid.GetSeparationForce() * boid.GetSeparationMultiplier());
			boid.ApplyForce(boid.GetAlignmentForce() * boid.GetAlignmentMultiplier());
			boid.ApplyForce(boid.GetCohesionForce() * boid.GetCohesionMultiplier());
			boid.ApplyForce(GetRepulsionBorderForce(boid));
		}

		for ( auto &boid : _boids )
		{
			boid.Update();
			boid.ComputeVisionShape();
		}

		_wantComputeNeighborLines = _renderNeighbors;
		_wantComputeVisibleNeighborLines = _renderVisibleNeighbors;
		_wantComputeVelocityLines = _renderVelocities;
		_wantComputeAccelerationLines = _renderAccelerations;
		_wantComputeActiveQuadtreeGrid = _renderQuadtree;
	}

	if ( _renderFlocks )
	{
		ComputeFlocks();
		_wantComputeFlockLines = true;
	}

	if ( !_paused || _renderFlocks || _renderNeighbors || _renderVisibleNeighbors )
	{
		ComputeAllVisibleNeighbors();
	}

	CheckRenderState();
}

void BoidManager::OnRender(Scene &scene)
{
	if ( _renderVision )
	{
		RenderSightShapes(scene);
	}

	if ( _renderNeighbors )
	{
		RenderNeighbors(scene);
	}

	if ( _renderVisibleNeighbors )
	{
		RenderVisibleNeighbors(scene);
	}

	if ( _renderVelocities )
	{
		RenderVelocities(scene);
	}

	if ( _renderAccelerations )
	{
		RenderAccelerations(scene);
	}

	if ( _renderBodies )
	{
		RenderBodies(scene);
	}

	if ( _renderQuadtree )
	{
		RenderQuadTree(scene);
	}

	if ( _renderFlocks )
	{
		RenderFlockConvexHull(scene);
	}
}

void BoidManager::OnGuiRender()
{
	if ( _paused )
	{
		if ( ImGui::Button("Resume", { ImGui::GetContentRegionAvailWidth(), 0.0f }) )
		{
			Resume();
		}
	}
	else
	{
		if ( ImGui::Button("Pause", { ImGui::GetContentRegionAvailWidth(), 0.0f }) )
		{
			Pause();
		}
	}
	Gui::BeginPropertyGrid("NoBoids");
	if ( Gui::Property("Boid Count", _noBoids, 1, 200, 1, Gui::PropertyFlag_Slider) )
	{
		SetBoidCount(_noBoids);
	}
	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid("Forces");
	if ( Gui::Property("Separation", _separationMultiplier, 0.0f, 10.0f, 0.1f, Gui::PropertyFlag_Slider) )
	{
		SetSeparationMultiplier(_separationMultiplier);
	}
	if ( Gui::Property("Alignment", _alignmentMultiplier, 0.0f, 10.0f, 0.1f, Gui::PropertyFlag_Slider) )
	{
		SetAlignmentMultiplier(_alignmentMultiplier);
	}
	if ( Gui::Property("Cohesion", _cohesionMultiplier, 0.0f, 10.0f, 0.1f, Gui::PropertyFlag_Slider) )
	{
		SetCohesionMultiplier(_cohesionMultiplier);
	}
	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid("Speed");
	if ( Gui::Property("Min Speed", _minSpeed, 0.0f, 1000.0f, 1.0f, Gui::PropertyFlag_Slider) )
	{
		SetMinSpeed(_minSpeed);
		if ( _minSpeed > _maxSpeed )
		{
			SetMaxSpeed(_maxSpeed = _minSpeed);
		}
	}
	if ( Gui::Property("Max Speed", _maxSpeed, 0.0f, 1000.0f, 1.0f, Gui::PropertyFlag_Slider) )
	{
		SetMaxSpeed(_maxSpeed);
		if ( _maxSpeed < _minSpeed )
		{
			SetMinSpeed(_minSpeed = _maxSpeed);
		}
	}
	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid("Vision");
	if ( Gui::Property("Vision Radius", _visionRadius, 0.0f, 400.0, 1.0f, Gui::PropertyFlag_Slider) )
	{
		SetVisionRadius(_visionRadius);
	}
	if ( Gui::Property("Vision Angle", _visionAngle, 0.0f, 360.0f, 1.0f, Gui::PropertyFlag_Slider) )
	{
		SetVisionAngle(_visionAngle);
	}
	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid("DrawCheckboxes");
	Gui::Property("Bodies", _renderBodies);
	Gui::Property("Vision", _renderVision);
	Gui::Property("Neighbors", _renderNeighbors);
	Gui::Property("VisibleNeighbors", _renderVisibleNeighbors);
	Gui::Property("Velocities", _renderVelocities);
	Gui::Property("Accelerations", _renderAccelerations);
	Gui::Property("Flocks", _renderFlocks);
	Gui::Property("Quadtree", _renderQuadtree);
	Gui::EndPropertyGrid();
}

void BoidManager::OnFirstRenderTargetResize(const sf::Vector2f &size)
{
	SetBoidCount(200);
}

void BoidManager::CheckRenderState()
{
	if ( _wantComputeNeighborLines )
	{
		ComputeNeighborLinesVA();
		_wantComputeNeighborLines = false;
	}

	if ( _wantComputeVisibleNeighborLines )
	{
		ComputeVisibleNeighborLinesVA();
		_wantComputeVisibleNeighborLines = false;
	}

	if ( _wantComputeVelocityLines )
	{
		ComputeVelocityLinesVA();
		_wantComputeVelocityLines = false;
	}

	if ( _wantComputeAccelerationLines )
	{
		ComputeAccelerationLinesVA();
		_wantComputeAccelerationLines = false;
	}

	if ( _wantComputeFlockLines )
	{
		ComputeFlockLinesVA();
		_wantComputeFlockLines = false;
	}

	if ( _wantComputeActiveQuadtreeGrid )
	{
		ComputeActiveQuadTreeGridVA();
		_wantComputeActiveQuadtreeGrid = false;
	}
}

void BoidManager::RenderBodies(Scene &scene) const
{
	for ( const auto &boid : _boids )
	{
		scene.Submit(boid.GetBodyShape());
	}
}

void BoidManager::RenderSightShapes(Scene &scene) const
{
	for ( auto &boid : _boids )
	{
		boid.RenderVision(scene);
	}
}

void BoidManager::RenderNeighbors(Scene &scene) const
{
	scene.Submit(_neighborLinesVA);
}

void BoidManager::RenderVisibleNeighbors(Scene &scene) const
{
	scene.Submit(_visibleNeighborLinesVA);
}

void BoidManager::RenderVelocities(Scene &scene) const
{
	scene.Submit(_velocityLinesVA);
}

void BoidManager::RenderAccelerations(Scene &scene) const
{
	scene.Submit(_accelerationLinesVA);
}

void BoidManager::RenderQuadTree(Scene &scene)
{
	scene.Submit(_quadtreeGridVA);
	scene.Submit(_activeQuadtreeGridVA);
}

void BoidManager::RenderFlockConvexHull(Scene &scene)
{
	scene.Submit(_flockLinesVA);
}

void BoidManager::Pause()
{
	_paused = true;
}

void BoidManager::Resume()
{
	_paused = false;
}

void BoidManager::SetBoidCount(int count)
{
	const int oldSize = _boids.size();
	while ( _boids.size() != count )
	{
		if ( _boids.size() < count )
		{
			const auto &[camTopLeft, camBottomRight] = _camera.GetViewport();

			const sf::Vector2f constrainedTopLeft{
				GenUtils::Constrain(camTopLeft.x, -_gridOffset.x, _gridOffset.x),
				GenUtils::Constrain(camTopLeft.y, -_gridOffset.y, _gridOffset.y) };
			const sf::Vector2f constrainedBottomRight{
				GenUtils::Constrain(camBottomRight.x, -_gridOffset.x, _gridOffset.x),
				GenUtils::Constrain(camBottomRight.y, -_gridOffset.y, _gridOffset.y) };

			sf::Vector2f randomPosition = Random::Vec2(constrainedTopLeft, constrainedBottomRight);
			_boids.push_back(Boid(randomPosition));
		}
		else
		{
			_boids.pop_back();
		}
	}
	if ( oldSize < count )
	{
		for ( int i = oldSize; i < _boids.size(); i++ )
		{
			sf::Vector2f randomForce;
			while ( (randomForce = Random::Vec2(-1.0f, -1.0f, 1.0f, 1.0f) * 1000.0f) == VecUtils::Null<>() )
				;
			_boids[i].ApplyForce(randomForce);
		}

		SetSeparationMultiplier(_separationMultiplier);
		SetAlignmentMultiplier(_alignmentMultiplier);
		SetCohesionMultiplier(_cohesionMultiplier);

		SetMinSpeed(_minSpeed);
		SetMaxSpeed(_maxSpeed);
	}
}

void BoidManager::SetSeparationMultiplier(float multiplier)
{
	for ( auto &boid : _boids )
	{
		boid.SetSeparationMultiplier(multiplier);
	}
}

void BoidManager::SetAlignmentMultiplier(float multiplier)
{
	for ( auto &boid : _boids )
	{
		boid.SetAlignmentMultiplier(multiplier);
	}
}

void BoidManager::SetCohesionMultiplier(float multiplier)
{
	for ( auto &boid : _boids )
	{
		boid.SetCohesionMultiplier(multiplier);
	}
}

void BoidManager::SetMinSpeed(float speed)
{
	for ( auto &boid : _boids )
	{
		boid.SetMinSpeed(speed);
	}
}

void BoidManager::SetMaxSpeed(float speed)
{
	for ( auto &boid : _boids )
	{
		boid.SetMaxSpeed(speed);
	}
}

void BoidManager::SetVisionRadius(float radius)
{
	for ( auto &boid : _boids )
	{
		boid.SetSightRadius(radius);
	}
}

void BoidManager::SetVisionAngle(float angle)
{
	for ( auto &boid : _boids )
	{
		boid.SetSightAngle(angle);
	}
}

void BoidManager::ComputeAllVisibleNeighbors()
{
	for ( const auto &[xIndex, yIndex] : _activeQuadtreeContainers )
	{
		for ( auto &boid : _quadtree[xIndex][yIndex] )
		{
			boid->ClearNeighbors();
			boid->ClearVisibleNeighbors();

			const int nLayersToCheck = static_cast<int>(std::floor(static_cast<int>(static_cast<float>(boid->GetSightRadius() / std::min(_quadtreeBox.width, _quadtreeBox.height)) * 2.0f) + 1));
			for ( int i = 0; i < nLayersToCheck + 2; i++ )
			{
				for ( int j = 0; j < nLayersToCheck + 2; j++ )
				{
					const int x = GenUtils::Constrain(xIndex - nLayersToCheck / 2 + i, 0, static_cast<int>(_quadtree.size() - 1));
					const int y = GenUtils::Constrain(yIndex - nLayersToCheck / 2 + j, 0, static_cast<int>(_quadtree[x].size() - 1));

					for ( auto &otherBoid : _quadtree[x][y] )
					{
						if ( boid == otherBoid )
							continue;

						const sf::Vector2f _thisPos = boid->GetPosition();
						const sf::Vector2f _otherPos = otherBoid->GetPosition();
						const sf::Vector2f forward = boid->GetForward();
						const auto &[lBOund, rBound] = boid->GetSightBounds();

						const bool inSeeingDistance = VecUtils::DistanceSq(_thisPos, _otherPos) < boid->GetSightRadius() * boid->GetSightRadius();
						const bool inPeripheral = !VecUtils::IsLeft(_thisPos, lBOund, _otherPos) && VecUtils::IsLeft(_thisPos, rBound, _otherPos);
						const bool inFront = VecUtils::IsLeft(VecUtils::Perpendicular(forward), _thisPos, _otherPos);
						const bool angleBiggerThan180 = boid->GetSightAngle() > 180.0f;
						const bool angleBiggerThan360 = boid->GetSightAngle() > 360.0f;

						if ( inSeeingDistance )
						{
							boid->AddNeighbor(otherBoid);
							if ( inPeripheral ||
								angleBiggerThan360 ||
								angleBiggerThan180 && inFront )
							{
								boid->AddVisibleNeighbor(otherBoid);
							}
						}
					}
				}
			}
		}
	}
}

void BoidManager::ComputeFlocks()
{
	_flocks.clear();
	_inFlock.clear();
	for ( auto &boid : _boids )
	{
		if ( _inFlock.find(boid) == _inFlock.end() )
		{
			_inFlock.emplace(boid);
			Set<Boid *> currentFlock;
			IterativeFlockCheck(boid, currentFlock);
			_flocks.push_back(currentFlock);
		}
	}
}

void BoidManager::IterativeFlockCheck(const Boid &boid, Set<Boid *> &currentFlock)
{
	for ( const auto &neighbor : boid.GetNeighbors() )
	{
		if ( currentFlock.emplace(const_cast<Boid *>(neighbor)).second )
		{
			_inFlock.emplace(*neighbor);
			IterativeFlockCheck(*neighbor, currentFlock);
		}
	}
}

sf::Vector2f BoidManager::GetRepulsionBorderForce(const Boid &boid) const
{
	const sf::Vector2f position = boid.GetPosition();
	const auto &box = _repulsionBorders;

	const auto xRange = CreatePair(box.left, box.left + box.width);
	const auto yRange = CreatePair(box.top, box.top + box.height);

	float fromLeft = GenUtils::Map(position.x, xRange, CreatePair(-1.0f, 1.0f));
	float fromTop = GenUtils::Map(position.y, yRange, CreatePair(-1.0f, 1.0f));
	float fromRight = -fromLeft;
	float fromBottom = -fromTop;

	fromLeft = GenUtils::Constrain(fromLeft, -100.0f, 1.0f);
	fromTop = GenUtils::Constrain(fromTop, -100.0f, 1.0f);
	fromRight = GenUtils::Constrain(fromRight, -100.0f, 1.0f);
	fromBottom = GenUtils::Constrain(fromBottom, -100.0f, 1.0f);

	sf::Vector2f force(0.0f, 0.0f);
	const float lowering = 1.0f;
	force.x += GenUtils::Constrain(std::pow((fromLeft - 1.0f) / 2.0f, 128.0f) - lowering, 0.0f, std::numeric_limits<float>::infinity());
	force.y += GenUtils::Constrain(std::pow((fromTop - 1.0f) / 2.0f, 128.0f) - lowering, 0.0f, std::numeric_limits<float>::infinity());
	force.x -= GenUtils::Constrain(std::pow((fromRight - 1.0f) / 2.0f, 128.0f) - lowering, 0.0f, std::numeric_limits<float>::infinity());
	force.y -= GenUtils::Constrain(std::pow((fromBottom - 1.0f) / 2.0f, 128.0f) - lowering, 0.0f, std::numeric_limits<float>::infinity());

	return force;
}

void BoidManager::ComputeQuadTree()
{
	for ( auto &boid : _boids )
	{
		const sf::Vector2f &position = boid.GetPosition();
		sf::Vector2i middleIndex((position.x - _quadtreeRect.left) / _quadtreeBox.width, (position.y - _quadtreeRect.top) / _quadtreeBox.height);

		middleIndex.x = GenUtils::Constrain(middleIndex.x, 0, static_cast<int>(_quadtree.size()) - 1);
		middleIndex.y = GenUtils::Constrain(middleIndex.y, 0, static_cast<int>(_quadtree[middleIndex.x].size()) - 1);

		_quadtree[middleIndex.x][middleIndex.y].push_back(&boid);
		_activeQuadtreeContainers.emplace(middleIndex.x, middleIndex.y);
	}
}

void BoidManager::ClearQuadtree()
{
	for ( const auto &[x, y] : _activeQuadtreeContainers )
	{
		_quadtree[x][y].clear();
	}
	_activeQuadtreeContainers.clear();
}

void BoidManager::ComputeNeighborLinesVA()
{
	ComputeNeighborsLinesVAHelper(_neighborLinesVA, _boids, false);
}

void BoidManager::ComputeVisibleNeighborLinesVA()
{
	ComputeNeighborsLinesVAHelper(_visibleNeighborLinesVA, _boids, true);
}

void BoidManager::ComputeVelocityLinesVA()
{
	ComputePhysicsLinesVAHelper(_velocityLinesVA, _boids, true);
}

void BoidManager::ComputeAccelerationLinesVA()
{
	ComputePhysicsLinesVAHelper(_accelerationLinesVA, _boids, false);
}

void BoidManager::ComputeFlockLinesVA()
{
	_flockLinesVA.clear();
	for ( const auto &flock : _flocks )
	{
		if ( flock.size() < 3 )
		{
			continue;
		}

		ArrayList<sf::Vector2f> flockPoints;
		for ( const auto &boid : flock )
		{
			flockPoints.push_back(boid->GetPosition());
		}

		ArrayList<sf::Vector2f> wrapped(GenUtils::WrapPoints(flockPoints));
		for ( auto &point : wrapped )
		{
			_flockLinesVA.append({ point, _flockLinesColor });
		}
	}
}

void BoidManager::ComputeActiveQuadTreeGridVA()
{
	_activeQuadtreeGridVA.clear();
	const int activeContainers = _activeQuadtreeContainers.size();

	const int noVertices = activeContainers * 4;
	if ( noVertices != _activeQuadtreeGridVA.getVertexCount() )
	{
		_activeQuadtreeGridVA.resize(noVertices);
	}

	const auto color = _activeQuadtreeContainerColor;
	const auto width = _quadtreeBox.width;
	const auto height = _quadtreeBox.height;

	int index = 0;
	for ( const auto &[x, y] : _activeQuadtreeContainers )
	{
		const sf::Vector2f topLeft = sf::Vector2f{ x * _quadtreeBox.width, y * _quadtreeBox.height } - _gridOffset;

		_activeQuadtreeGridVA[index] = { topLeft, color };
		_activeQuadtreeGridVA[index + 1] = { sf::Vector2f{topLeft.x + width, topLeft.y}, color };
		_activeQuadtreeGridVA[index + 2] = { sf::Vector2f{topLeft.x + width, topLeft.y + height}, color };
		_activeQuadtreeGridVA[index + 3] = { sf::Vector2f{topLeft.x, topLeft.y + height}, color };

		index += 4;
	}
}

void BoidManager::ComputeNeighborsLinesVAHelper(sf::VertexArray &va, const ArrayList<Boid> &boids, bool onlyVisible)
{
	va.clear();
	auto totalNeighbors = 0;
	for ( const auto &boid : boids )
	{
		totalNeighbors += onlyVisible ? boid.GetVisibleNeighbors().size() : boid.GetNeighbors().size();;
	}

	const auto noVerticies = totalNeighbors * 2;
	if ( noVerticies != va.getVertexCount() )
	{
		va.resize(noVerticies);
	}

	int index = 0;
	for ( const auto &boid : boids )
	{
		const auto &neighbors = onlyVisible ? boid.GetVisibleNeighbors() : boid.GetNeighbors();
		const auto color = onlyVisible ? boid.GetVisibleNeighborLinesColor() : boid.GetNeighborLinesColor();

		int neighborIndex = 0;
		for ( const auto *neighbor : neighbors )
		{
			va[index + neighborIndex] = { boid.GetPosition(), color };
			va[index + neighborIndex + 1] = { neighbor->GetPosition(), color };
			neighborIndex += 2;
		}
		index += neighbors.size() * 2;
	}
}

void BoidManager::ComputePhysicsLinesVAHelper(sf::VertexArray &va, const ArrayList<Boid> &boids, bool velocity)
{
	va.clear();
	const auto noVerticies = boids.size() * 2;
	if ( noVerticies != va.getVertexCount() )
	{
		va.resize(noVerticies);
	}

	int index = 0;
	for ( const auto &boid : boids )
	{
		const auto position = boid.GetPosition();
		const auto color = velocity ? boid.GetVelocityLineColor() : boid.GetAccelerationLineColor();
		const auto extend = velocity ? boid.GetVelocity() : (boid.GetAcceleration() * 0.05f);

		va[index] = { position, color };
		va[index + 1] = { position + extend, color };

		index += 2;
	}
}
}

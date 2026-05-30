#pragma once

#include <memory>

#include "layers/base_layer.h"

#include "boid_manager.h"

namespace flocking
{
using namespace saffron;
class ProjectLayer : public BaseLayer
{
public:
	void OnAttach(std::shared_ptr<Batch> &loader) override;
	void OnDetach() override;

	void OnUpdate() override;
	void OnGuiRender() override;

	void OnRenderTargetResize(const sf::Vector2f &newSize) override;

private:
	std::unique_ptr<BoidManager> _boidManager;
	bool _firstRenderTargetResize = true;
};
}

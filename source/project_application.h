#pragma once

#include <memory>

#include <saffron.h>

#include "layers/project_layer.h"

namespace flocking
{
using namespace saffron;
class ProjectApplication : public App
{
public:
	explicit ProjectApplication(const Properties &properties);

	void OnInit() override;

	void OnUpdate() override;

private:
	std::shared_ptr<ProjectLayer> _projectLayer;
};
}
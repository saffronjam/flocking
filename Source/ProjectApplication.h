#pragma once

#include <Saffron.h>

#include "Layers/ProjectLayer.h"

namespace Se
{
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
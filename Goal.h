#include "GameObject.h"
#pragma once

namespace gef
{
	class Mesh;
}

class Goal : public GameObject
{
public:
	Goal(gef::Platform& platform);
	~Goal();

	void Init();

	gef::Mesh* goal_mesh_;
	gef::Scene* goal_scene_;
	
	gef::Aabb bounds_;

};


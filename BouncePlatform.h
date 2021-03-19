#pragma once
#include "GameObject.h"

namespace gef
{
	class Mesh;
}

class BouncePlatform : public GameObject
{
public:
	BouncePlatform(gef::Platform& platform);
	~BouncePlatform();

	void Init();

	bool Update(float frame_time);

	void RandomizeScale();

	gef::Mesh* platform_mesh_;
	gef::Scene* platform_scene_;

	gef::Aabb bounds_;
private:

	float z_pos_;
	float z_pos_acceleration_;

	bool float_up_;
};


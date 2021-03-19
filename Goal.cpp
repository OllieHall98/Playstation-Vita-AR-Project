#include "Goal.h"
#include "graphics\mesh.h"

Goal::Goal(gef::Platform& platform) : GameObject(platform)
{
	Init();
}


Goal::~Goal()
{
}

void Goal::Init()
{
	ReadAndAssignFirstMesh("goal2.scn", &goal_scene_, &goal_mesh_);
	set_mesh(goal_mesh_);

	float scale = 0.06f;

	bounds_.set_min_vtx(gef::Vector4(-0.58f, -0.58f, -0.60f));
	bounds_.set_max_vtx(gef::Vector4(0.58, 0.58, 0.55));
	goal_mesh_->set_aabb(bounds_);

	transform_.SetScale(gef::Vector4(scale, scale, scale * 0.1f));
	transform_.SetZPosition(0.01f);

	

	SetParent(marker_transform_);
}


#include "BouncePlatform.h"
#include "graphics\mesh.h"

BouncePlatform::BouncePlatform(gef::Platform& platform) : GameObject(platform)
{
	Init();
}

BouncePlatform::~BouncePlatform()
{

}

void BouncePlatform::Init()
{
	ReadAndAssignFirstMesh("platform.scn", &platform_scene_, &platform_mesh_);
	set_mesh(platform_mesh_);
	
	float scale = 0.75f;

	bounds_.set_min_vtx(gef::Vector4(-1.89f * scale, -2.96f * scale, -0.31 * scale));
	bounds_.set_max_vtx(gef::Vector4(1.91f * scale, 2.98f * scale, 0.23 * scale));
	platform_mesh_->set_aabb(bounds_);

	transform_.SetScale(gef::Vector4(0.02f, 0.02f, 0.02f));
	transform_.SetZPosition(0.01f);

	SetParent(marker_transform_);

	z_pos_acceleration_ = 0.001f;
}

bool BouncePlatform::Update(float frame_time)
{
	GameObject::Update(frame_time);

	z_pos_ += z_pos_acceleration_;

	if (z_pos_acceleration_ > 0.0005f) z_pos_acceleration_ = 0.0005f;
	if (z_pos_acceleration_ < -0.0005f) z_pos_acceleration_ = -0.0005f;


	if (float_up_)
	{
		z_pos_acceleration_ += 0.001f * frame_time;
	}
	else
	{
		z_pos_acceleration_ -= 0.001f * frame_time;
	}

	if (z_pos_ >= 0.0001f) float_up_ = false;
	if (z_pos_ <= -0.0001f) float_up_ = true;


	transform_.SetZPosition(z_pos_);

	return true;

}

void BouncePlatform::RandomizeScale()
{
	float scale = (rand() % 1 + 1) * 0.02f;

	transform_.SetScale(gef::Vector4(scale, scale, scale * 0.1f));
}
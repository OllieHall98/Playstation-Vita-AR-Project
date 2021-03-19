#include "graphics\mesh_instance.h"
#include "MyTransform.h"
#include <graphics/scene.h>
#include "GlobalInput.h"
#pragma once

namespace gef
{
	class Renderer3D;
};

class GameObject : public gef::MeshInstance
{
public:
	GameObject(gef::Platform& platform);
	~GameObject();

	void Init(gef::Platform& platform);
	bool Update(float deltaTime);

	void SetParent(MyTransform* parent);

	bool IsColliding_SphereToSphere(const gef::MeshInstance& meshInstance1, const gef::MeshInstance& meshInstance2);
	bool IsColliding_AABBToAABB(const gef::MeshInstance& meshInstance1, const gef::MeshInstance& meshInstance2);
	void RandomizeScale(float scale);
	void RandomizeOffset();
	bool IsColliding_SphereToAABB(const gef::MeshInstance& meshInstance1, const gef::MeshInstance& meshInstance2);


	void Render(gef::Renderer3D* renderer);

	void ReadAndAssignFirstMesh(const char * filename, gef::Scene ** scene, gef::Mesh ** mesh);

	gef::Mesh * GetFirstMesh(gef::Scene * scene);

	gef::Scene** scene_;

	gef::Platform& platform_;

	GlobalInput* player_input_;

	bool render_ = false;

	MyTransform* marker_transform_;

	MyTransform transform_;

};


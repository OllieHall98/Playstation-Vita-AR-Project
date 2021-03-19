#include "GameObject.h"
#include <graphics/renderer_3d.h>
#include "maths\sphere.h"
#include "graphics\mesh.h"




GameObject::GameObject(gef::Platform& platform) :
platform_(platform),
player_input_(NULL)
{
	Init(platform_);
}

GameObject::~GameObject()
{
	player_input_ = NULL;
}

void GameObject::Init(gef::Platform& platform)
{
	marker_transform_ = new MyTransform();

	transform_.InitTransform();

	player_input_ = new GlobalInput(platform_);
}

bool GameObject::Update(float deltaTime)
{
	transform_.UpdateTransform();

	// Set the transform of the mesh
	set_transform(transform_.GetWorldTransform());

	return true;
}

void GameObject::SetParent(MyTransform* parent)
{
	transform_.SetParent(parent);
}

void GameObject::Render(gef::Renderer3D* renderer)
{
	//renderer->DrawMesh();
	//renderer->DrawMesh(mesh_);
}

void GameObject::ReadAndAssignFirstMesh(const char* filename, gef::Scene** scene, gef::Mesh** mesh)
{
	gef::Scene* scn = new gef::Scene;
	scn->ReadSceneFromFile(platform_, filename);

	scn->CreateMaterials(platform_);

	*mesh = GetFirstMesh(scn);
	*scene = scn;
}

gef::Mesh* GameObject::GetFirstMesh(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	if (scene)
	{
		// now check to see if there is any mesh data in the file, if so lets create a mesh from it
		if (scene->mesh_data.size() > 0)
			mesh = scene->CreateMesh(platform_, scene->mesh_data.front());
	}

	return mesh;;
}

bool GameObject::IsColliding_SphereToSphere(const gef::MeshInstance& meshInstance1, const gef::MeshInstance& meshInstance2)
{
	// Get the bounding spheres of both objects to check for collision
	gef::Sphere sphere1 = meshInstance1.mesh()->bounding_sphere();
	gef::Sphere sphere2 = meshInstance2.mesh()->bounding_sphere();

	// Get the transforms of the objects too
	// We will need both the transform and bounding sphere to calculate if there is an intersection
	gef::Sphere sphere1_transformed = sphere1.Transform(meshInstance1.transform());
	gef::Sphere sphere2_transformed = sphere2.Transform(meshInstance2.transform());

	// Calculate the difference in the positions between the two objects
	gef::Vector4 offset = sphere1_transformed.position() - sphere2_transformed.position();

	// Pythagoras - Square root of 'A2 + B2 + C2' results in a single scalar value denoting the distance
	float distance = sqrtf(offset.x() * offset.x() + offset.y() * offset.y() + offset.z() + offset.z());

	// The combined radius of both objects. 
	// If the distance is smaller than the combined radii, the objects are intersecting
	float combined_radii = sphere1_transformed.radius() + sphere2_transformed.radius();

	return distance < combined_radii;
}

bool GameObject::IsColliding_AABBToAABB(const gef::MeshInstance& meshInstance1, const gef::MeshInstance& meshInstance2)
{
	// Get the bounding spheres of both objects to check for collision
	gef::Aabb AABB1 = meshInstance1.mesh()->aabb();
	gef::Aabb AABB2 = meshInstance2.mesh()->aabb();

	// Get the transforms of the objects too
	// We will need both the transform and bounding boxes to calculate if there is an intersection
	gef::Aabb AABB1_t = AABB1.Transform(meshInstance1.transform());
	gef::Aabb AABB2_t = AABB2.Transform(meshInstance2.transform());


	if (
		// If the maximum x value of one box (right) is greater than the minumum of the other (left)
		AABB1_t.max_vtx().x() > AABB2_t.min_vtx().x() &&
		AABB1_t.min_vtx().x() < AABB2_t.max_vtx().x() &&

		AABB1_t.max_vtx().y() > AABB2_t.min_vtx().y() &&
		AABB1_t.min_vtx().y() < AABB2_t.max_vtx().y() &&

		AABB1_t.max_vtx().z() > AABB2_t.min_vtx().z() &&
		AABB1_t.min_vtx().z() < AABB2_t.max_vtx().z()

		)
	{
		return true;
	}

	return false;
}

void GameObject::RandomizeScale(float objectscale)
{
	float scale = (rand() % 2 + 3) * objectscale;

	transform_.SetScale(gef::Vector4(scale, scale, scale * 0.1f));
}

void GameObject::RandomizeOffset()
{
	float offsetX = (rand() % 2 + 1) * 0.01f;
	float offsetY = (rand() % 2 + 1) * 0.01f;

	transform_.SetXPosition(offsetX);
	transform_.SetYPosition(offsetY);
}

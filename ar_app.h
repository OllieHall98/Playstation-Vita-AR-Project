#ifndef _AR_APP_H
#define _AR_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>

namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class baseState;

class ARApp : public gef::Application
{
public:
	ARApp(gef::Platform& platform);
	void Init();
	bool Update(float frameTime);
	void CleanUp();
	void Render();

private:
	void initialiseFont();

	gef::SpriteRenderer* spriteRenderer;
	gef::Font* font;
	gef::Renderer3D* renderer3d;

	PrimitiveBuilder* primitiveBuilder;

	baseState* currentState;
	baseState* nextState;

	float fps_;
};

#endif // _SCENE_APP_H

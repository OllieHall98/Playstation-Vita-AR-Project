#ifndef _RENDER_TARGET_APP_H
#define _RENDER_TARGET_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <platform/vita/graphics/texture_vita.h>
#include "primitive_builder.h"
#include "GameObject.h"
#include "GlobalInput.h"
#include "Cannon.h"
#include "BouncePlatform.h"
#include "Goal.h"
#include "states.h"
#include "audio\audio_manager.h"

// Vita AR includes
#include <camera.h>
#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class RenderTarget;
	class TextureVita;
	class InputManager;
	class Scene;
	class GameObject;
}

enum LevelState { MENU, LEVEL };


class Level : public baseState
{
public:
	Level(gef::Platform& platform);
	~Level();
	void onEnter();
	void InitCannon();
	void InitCameraSprite();
	void InitSprites();
	void InitAudio();
	void onExit();
	void CleanUp();
	void PlaySFX(int sfx);
	void PlaySFX(int sfx, float volume, float pitch);
	baseState* onUpdate(float frame_time);
	void EndGame();
	void CalculateCannonGoalDistance();
	void SetPowerMeterNotchPosition();
	void CheckForMarker(int markerNum, GameObject* object, float frame_time);
	void CheckForMarker(int markerNum, BouncePlatform * object, float frame_time);
	void CheckForMarker(int markerNum, Cannon* object, float frame_time);
	void DetermineCannonGoalDistance();
	void NextStage(int new_score_);
	void onRender();
	void RenderGameObjects();
	void CreateARMatrices();
private:
	void InitFont();
	void CleanUpFont();
	void DrawPowerMeter();
	void DrawGameOverScreen();
	void DrawTimer();
	void DrawDistanceWarning();
	void DrawMenuOverlays();
	void DrawHUD();
	void RenderOverlay();
	void SetupLights();
	void UpdateMarker(MyTransform* markerTransform, int cardNumber);
	void HandlePlayerInput(float frametime);

	float fps_;

	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	class gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	GlobalInput* player_input_;
	gef::Platform& platform_;
	gef::AudioManager* audio_manager_;
	gef::TextureVita* camera_texture_;
	gef::Scene* model_scene_;

#pragma region Objects

	Cannon* cannon;
	BouncePlatform* bounce_platform1;
	BouncePlatform* bounce_platform2;
	Goal* goal;

#pragma endregion

#pragma region Sprites

	gef::Sprite* camera_sprite_;
	float camera_vertical_scale_;

	gef::Sprite power_meter_sprite_;
	gef::Texture* power_meter_texture_;

	gef::Sprite power_meter_notch_sprite_;
	gef::Texture* power_meter_notch_texture_;

	gef::Sprite pause_menu_sprite_;
	gef::Texture* pause_menu_texture_;

	gef::Sprite loading_sprite_;
	gef::Texture* loading_texture_;

#pragma endregion

#pragma region Audio

	int music_;
#pragma endregion

	float cannon_goal_distance_;
	float cannon_goal_desired_distance_ = 0.25f;
	int score_;

	float timer_;

	bool game_over_ = false;
	bool return_to_menu_ = false;
	bool paused_ = false;
	bool pause_button_released_ = false;

};


#endif // _RENDER_TARGET_APP_H
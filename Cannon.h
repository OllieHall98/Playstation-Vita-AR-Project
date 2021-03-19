#pragma once
#include "GameObject.h"
#include "Goal.h"
#include "BouncePlatform.h"
#include "audio\audio_manager.h"

namespace gef
{
	class Mesh;
}

class Cannon
{
public:
	Cannon(gef::Platform& platform, gef::AudioManager* audio_manager);
	~Cannon();

	void Init(gef::Platform& platform);

	void InitCannonball(float scale, gef::Scene * &scn);

	void InitCannonLauncher(gef::Platform & platform, gef::Scene * &scn);

	void InitCannonBase(gef::Platform & platform, gef::Scene * &scn);

	void InitAudio(gef::Platform& platform);

	void SetParents(MyTransform* parent);

	void SetCannonBallParent(GameObject* parent);

	void Update(GlobalInput* player_input_, float frame_time);

	void DetermineIfCanControl();

	void MoveBall(float frame_time);

	void PlaySFX(int sfx);

	void PlaySFX(int sfx, float volume, float pitch);

	void ResetBall();

	void FireCannon();

	void SetNewCannonBallPosition();

	void HandleInput(GlobalInput* player_input_, float frame_time);

	void HandleCannonControls(GlobalInput * player_input_, float frame_time);

	void ApplyCannonRotation();

	void TurnCannonRight();

	void TurnCannonLeft();

	void PitchCannonDown(float frame_time);

	void PitchCannonUp(float frame_time);

	void DecelerateTurn(float frame_time);

	bool NotTurningCannon();

	void BounceBall();

	void ChargeCannonFire(GlobalInput* player_input_ ,float frame_time);

	bool CollisionDetection(BouncePlatform object);

	int CollisionDetection(Goal object);

	void PlaceBallAtLaunchPosition();

	gef::Mesh* cannon_base_mesh_;
	gef::Mesh* cannon_launcher_mesh_;

	GameObject* cannon_launcher_;
	GameObject* cannon_base_;
	GameObject* cannon_ball_;

	Goal* goal_;

	gef::Vector4 ball_direction_;
	gef::Vector4 ball_velocity_;

#pragma region CannonMovement

	bool cannon_turning_ = false;
	bool can_control_ = true;

	float cannon_x_turn_speed_ = 0.0f;
	float cannon_z_turn_speed_ = 0.025f;
	float cannon_turn_max_ = 0.02f;
	float cannon_turn_acceleration_ = 0.05f;

#pragma endregion

	float launch_z_position;
	float cannon_scale = 0.015f;
	bool ball_shot_ = false;
	float gravity_scale_ = -9.8f;
	float gravity_ = 0;

	gef::Vector4 velocity_;

	bool render_ = false;

	MyTransform* marker_transform_;

	float cannon_charge_power_ = 2.5;
	float cannon_fire_power_ = 2.5;
	bool cannon_increase_power_ = true;
	float speed_scale_;

	float speed_;

	bool far_from_goal_ = false;
	bool first_shot_fired_ = false;

	int bounces = 0;

	gef::AudioManager* audio_manager_;

	int sfx_fire_;
	int sfx_bounce_;
	int sfx_miss_;
	int sfx_goal_;
	int sfx_charge_;
};


#include "Cannon.h"
#include "primitive_builder.h"
#include "graphics\mesh.h"
#include "system\debug_log.h"
#include "loadTexture.h"

Cannon::Cannon(gef::Platform& platform, gef::AudioManager* audiomanager) :
	audio_manager_(audiomanager)
{
	Init(platform);
}

Cannon::~Cannon()
{
}

void Cannon::Init(gef::Platform& platform)
{
	gef::Scene* scn;

	marker_transform_ = new MyTransform();

	InitCannonBase(platform, scn);
	InitCannonLauncher(platform, scn);

	PrimitiveBuilder* primitiveBuilder = new PrimitiveBuilder(platform);
	cannon_ball_ = new GameObject(platform);

	float scale = 0.75f;

	InitCannonball(scale, scn);

	PlaceBallAtLaunchPosition();

	SetParents(marker_transform_);

	cannon_charge_power_ = 2.5f;
	speed_scale_ = 5;
	speed_ = 0;
	bounces = 0;

	ball_shot_ = false;
	can_control_ = false;
	far_from_goal_ = false;

	InitAudio(platform);
}

void Cannon::InitCannonball(float scale, gef::Scene * &scn)
{
	gef::Aabb bounds;
	bounds.set_min_vtx(gef::Vector4(-0.225f * scale, -0.225f * scale, -0.225f * scale));
	bounds.set_max_vtx(gef::Vector4(0.225f * scale, 0.225f * scale, 0.225f * scale));

	gef::Mesh* ballMesh;
	cannon_ball_->ReadAndAssignFirstMesh("cannon_ball.scn", &scn, &ballMesh);
	cannon_ball_->transform_.SetScale(gef::Vector4(scale, scale, scale / 2));
	ballMesh->set_aabb(bounds);
	cannon_ball_->set_mesh(ballMesh);
}

void Cannon::InitCannonLauncher(gef::Platform & platform, gef::Scene * &scn)
{
	cannon_launcher_ = new GameObject(platform);
	cannon_launcher_->ReadAndAssignFirstMesh("cannon_launcher.scn", &scn, &cannon_launcher_mesh_);
	cannon_launcher_->set_mesh(cannon_launcher_mesh_);
	cannon_launcher_->transform_.SetScale(gef::Vector4(cannon_scale, cannon_scale, cannon_scale * 2));
	cannon_launcher_->transform_.SetZPosition(0.05f);
}

void Cannon::InitCannonBase(gef::Platform & platform, gef::Scene * &scn)
{
	cannon_base_ = new GameObject(platform);
	cannon_base_->ReadAndAssignFirstMesh("cannon_base.scn", &scn, &cannon_base_mesh_);
	cannon_base_->set_mesh(cannon_base_mesh_);
	cannon_base_->transform_.SetScale(gef::Vector4(cannon_scale, cannon_scale, cannon_scale * 2));
	cannon_base_->transform_.SetZPosition(0.025f);
}

void Cannon::InitAudio(gef::Platform& platform_)
{
	if (audio_manager_ == NULL)
		return;

	sfx_fire_ = audio_manager_->LoadSample("audio/fire.wav", platform_);
	sfx_bounce_ = audio_manager_->LoadSample("audio/bounce.wav", platform_);
	sfx_miss_ = audio_manager_->LoadSample("audio/miss.wav", platform_);
	sfx_goal_ = audio_manager_->LoadSample("audio/goal.wav", platform_);
	sfx_charge_ = audio_manager_->LoadSample("audio/charge.wav", platform_);
}

void Cannon::SetParents(MyTransform* parent)
{
	cannon_base_->transform_.SetParent(parent);
	cannon_launcher_->transform_.SetParent(parent);

	if (!ball_shot_)
	{
		SetCannonBallParent(cannon_launcher_);
	}
}

void Cannon::SetCannonBallParent(GameObject* parent)
{
	cannon_ball_->transform_.SetParent(&parent->transform_);

	gef::Vector4 launch_position_ = cannon_launcher_->transform_.GetLocalTransform().GetTranslation() + gef::Vector4(0, 0.0f, 1.0f);
	gef::Vector4 world_launch_position_ = launch_position_ - cannon_base_->transform_.GetWorldTransform().GetTranslation();
	cannon_ball_->transform_.SetPosition(world_launch_position_);
}

void Cannon::Update(GlobalInput* player_input_, float frame_time)
{
	DetermineIfCanControl();

	if (ball_shot_)
	{
		MoveBall(frame_time);
	}

	if (player_input_) HandleInput(player_input_, frame_time);

	cannon_base_->Update(frame_time);
	cannon_launcher_->Update(frame_time);
	cannon_ball_->Update(frame_time);
}

void Cannon::DetermineIfCanControl()
{
	if (far_from_goal_)
		can_control_ = true;
	else
		can_control_ = false;
}

void Cannon::MoveBall(float frame_time)
{
	// increase the "gravity" value
	gravity_ += gravity_scale_ * 0.15f * frame_time;

	// Determine the current speed of the ball based on the initial charge power and a fixed scale
	speed_ = cannon_charge_power_ * speed_scale_ * frame_time;

	// Determine the direction of the ball
	float xDirection = cos(cannon_launcher_->transform_.GetRotation().x());
	float yDirection = sin(cannon_launcher_->transform_.GetRotation().x());

	// Calculate the velocity based on the directional and speed values, including gravity
	velocity_ = gef::Vector4(0, (-yDirection * speed_), xDirection * speed_ + gravity_);

	// Change the position of the cannonball based on the velocity
	cannon_ball_->transform_.SetPosition(cannon_ball_->transform_.GetPosition() + velocity_);

	// Determine if the cannonball has fallen below the other objects
	if (cannon_ball_->transform_.GetWorldTransform().GetTranslation().y() < -0.1f)
	{
		PlaySFX(sfx_miss_);
		ResetBall();
	}

}

void Cannon::PlaySFX(int sfx)
{
	if (!audio_manager_)
		return;

	audio_manager_->PlaySample(sfx);
}


void Cannon::PlaySFX(int sfx, float volume, float pitch)
{
	// Play sound effects with custom volume and pitch

	if (audio_manager_ == NULL)
		return;

	gef::VolumeInfo volumeInfo;
	volumeInfo.volume = volume;

	audio_manager_->SetSamplePitch(sfx, pitch);

	audio_manager_->SetSampleVoiceVolumeInfo(sfx, volumeInfo);
	audio_manager_->PlaySample(sfx);
}

void Cannon::ResetBall()
{
	// Reset all values so the ball can be fired again

	SetCannonBallParent(cannon_launcher_);

	cannon_charge_power_ = 2.5f;
	cannon_increase_power_ = true;

	bounces = 0;
	speed_ = 0;
	speed_scale_ = 5;

	ball_shot_ = false;

	if (far_from_goal_)
		can_control_ = true;
	else
		can_control_ = false;
}

void Cannon::PlaceBallAtLaunchPosition()
{
	gef::Vector4 launch_position_ = cannon_launcher_->transform_.GetLocalTransform().GetTranslation() + gef::Vector4(0, 0.0f, 1.0f);
	gef::Vector4 world_launch_position_ = launch_position_ - cannon_base_->transform_.GetWorldTransform().GetTranslation();
	cannon_ball_->transform_.SetPosition(world_launch_position_);
}

void Cannon::BounceBall()
{
	// Play a sound effect for the ball bouncing
	PlaySFX(sfx_bounce_, 1.0f, (rand() % 10 + 90) / 100);

	bounces++;

	gravity_ = 0;

	// Reduce the speed of the ball for each bounce
	if(speed_scale_ - 0.5f > 0.5f)
		speed_scale_ -= 0.5f;
}

bool Cannon::CollisionDetection(BouncePlatform object)
{
	if (cannon_ball_->IsColliding_AABBToAABB(*cannon_ball_, object))
	{
		// Bounce the ball off of the platform
		BounceBall();

		return true;
	}

	return false;
}

int Cannon::CollisionDetection(Goal object)
{
	// If the cannonball collides with the goal AND it is moving (this prevents any "goals" before the game starts)
	if (cannon_ball_->IsColliding_AABBToAABB(*cannon_ball_, object) && speed_ != 0)
	{
		// Only count the score if the ball is being rendered
		if (object.render_ == false)
			return 0;

		// Play goal sfx
		PlaySFX(sfx_goal_, 1.0f, 1.0f);

		int score = 1 + bounces;

		ResetBall();

		return score;
	}

	return 0;
}


void Cannon::FireCannon()
{
	if (ball_shot_)
		return;

	if(!first_shot_fired_) 
		first_shot_fired_ = true;

	SetCannonBallParent(cannon_base_);
	SetNewCannonBallPosition();

	can_control_ = false;
	gravity_ = 0;

	PlaceBallAtLaunchPosition();
	velocity_ = gef::Vector4(0, 0, 1);

	cannon_increase_power_ = true;
	ball_shot_ = true;

	// Stop playing the charging sound effect
	audio_manager_->StopPlayingSampleVoice(sfx_charge_);

	// Play a sound effect for firing the cannonball
	PlaySFX(sfx_fire_);
}

void Cannon::SetNewCannonBallPosition()
{
	float offsetX = cannon_base_->transform_.GetPosition().x() + (cannon_launcher_->transform_.GetWorldTransform().GetTranslation().x() - cannon_base_->transform_.GetWorldTransform().GetTranslation().x());
	float offsetY = cannon_base_->transform_.GetPosition().y() + (cannon_launcher_->transform_.GetWorldTransform().GetTranslation().y() - cannon_base_->transform_.GetWorldTransform().GetTranslation().y());
	float offsetZ = cannon_base_->transform_.GetPosition().z() + (cannon_launcher_->transform_.GetWorldTransform().GetTranslation().z() - cannon_base_->transform_.GetWorldTransform().GetTranslation().z());

	cannon_ball_->transform_.SetPosition(gef::Vector4(offsetX, offsetY, offsetZ));
}

void Cannon::ChargeCannonFire(GlobalInput* player_input_, float frame_time)
{
	if (ball_shot_)
		return;

	// Alter the cannon charge power up or down
	cannon_charge_power_ += 7.5f * frame_time * (cannon_increase_power_ ? 1 : -1);

	// Determine if the cannon's power should increase or decrease based on its current value
	if (cannon_charge_power_ >= 7.5f)
		cannon_increase_power_ = false;
	else if (cannon_charge_power_ <= 2.5f)
		cannon_increase_power_ = true;

	// Play a sound effect as the cannon is being charged
	PlaySFX(sfx_charge_, 1.0f, cannon_charge_power_);
}


void Cannon::HandleInput(GlobalInput* player_input_, float frame_time)
{
	if (player_input_->IsInputPressed("Bounce"))
		BounceBall();

	if (!can_control_)
		return;

	HandleCannonControls(player_input_, frame_time);
}

void Cannon::HandleCannonControls(GlobalInput * player_input_, float frame_time)
{
	cannon_turning_ = false;

	if (player_input_->IsInputDown("Up"))
		PitchCannonUp(frame_time);

	if (player_input_->IsInputDown("Down"))
		PitchCannonDown(frame_time);

	if (player_input_->IsInputDown("Left"))
		TurnCannonLeft();

	if (player_input_->IsInputDown("Right"))
		TurnCannonRight();

	if (NotTurningCannon())
		DecelerateTurn(frame_time);

	ApplyCannonRotation();

	if (player_input_->IsInputDown("Fire"))
	{
		ChargeCannonFire(player_input_, frame_time);
	}
	else if (player_input_->IsInputUp("Fire"))
	{
		FireCannon();
	}
}

void Cannon::ApplyCannonRotation()
{
	cannon_launcher_->transform_.SetXRotation(cannon_launcher_->transform_.GetRotation().x() + cannon_x_turn_speed_);
}

void Cannon::TurnCannonRight()
{
	// Turn both the base and launcher of the cannon on the z axis
	cannon_launcher_->transform_.SetZRotation(cannon_launcher_->transform_.GetRotation().z() - cannon_z_turn_speed_);
	cannon_base_->transform_.SetZRotation(cannon_base_->transform_.GetRotation().z() - cannon_z_turn_speed_);
}

void Cannon::TurnCannonLeft()
{
	// Turn both the base and launcher of the cannon on the z axis
	cannon_launcher_->transform_.SetZRotation(cannon_launcher_->transform_.GetRotation().z() + cannon_z_turn_speed_);
	cannon_base_->transform_.SetZRotation(cannon_base_->transform_.GetRotation().z() + cannon_z_turn_speed_);
}

void Cannon::PitchCannonDown(float frame_time)
{
	// Don't allow the cannon to pitch down past a certain point
	if (cannon_launcher_->transform_.GetRotation().x() < -0.6)
	{
		cannon_x_turn_speed_ = 0;
		return;
	}

	// Apply acceleration to the speed to smooth the movement
	cannon_x_turn_speed_ -= (cannon_turn_acceleration_ * frame_time);

	cannon_turning_ = true;
}

void Cannon::PitchCannonUp(float frame_time)
{
	// Don't allow the cannon to pitch up past a certain point
	if (cannon_launcher_->transform_.GetRotation().x() > 0.6)
	{
		cannon_x_turn_speed_ = 0;
		return;
	}

	// Apply acceleration to the speed to smooth the movement
	cannon_x_turn_speed_ += (cannon_turn_acceleration_ * frame_time);

	cannon_turning_ = true;
}

void Cannon::DecelerateTurn(float frame_time)
{
	// Decelerate the turn if the player is not currently turning
	if (cannon_x_turn_speed_ > 0) cannon_x_turn_speed_ -= (cannon_turn_acceleration_ * frame_time);
	if (cannon_x_turn_speed_ < 0) cannon_x_turn_speed_ += (cannon_turn_acceleration_ * frame_time);
}

bool Cannon::NotTurningCannon()
{
	// If the player is NOT turning the cannon, slow down the turn acceleration
	return !cannon_turning_ && cannon_x_turn_speed_ != 0;
}



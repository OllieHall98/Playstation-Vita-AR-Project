#include "Level.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <input/input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/render_target.h>
#include <graphics/scene.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>
#include "loadTexture.h"
#include "Menu.h"

Level::Level(gef::Platform& platform) :
	platform_(platform),
	//input_manager_(NULL),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL),
	model_scene_(NULL),
	primitive_builder_(NULL),
	audio_manager_(NULL),
	cannon(NULL)
{
}

Level::~Level()
{
}

void Level::onEnter()
{
	//input_manager_ = gef::InputManager::Create(platform_);
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	primitive_builder_ = new PrimitiveBuilder(platform_);
	player_input_ = new GlobalInput(platform_);
	audio_manager_ = gef::AudioManager::Create(); // initialise audio manager

	InitFont();
	SetupLights();

	camera_vertical_scale_ = (960 / 544) / (640 / 480);

	// initialise sony framework
	sampleInitialize();
	smartInitialize();

	// reset marker tracking
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);

	InitCameraSprite();

	goal = new Goal(platform_);
	bounce_platform1 = new BouncePlatform(platform_);
	bounce_platform2 = new BouncePlatform(platform_);

	InitCannon();
	InitAudio();
	InitSprites();

	score_ = 0;
	timer_ = 25;
	
	game_over_ = false;
	return_to_menu_ = false;

	paused_ = false;
	pause_button_released_ = true;

	music_ = audio_manager_->LoadSample("audio/music.wav", platform_);

	audio_manager_->PlaySample(music_);
}

void Level::InitCannon()
{
	cannon = new Cannon(platform_, audio_manager_);
	cannon->goal_ = goal;
	cannon->first_shot_fired_ = false;
}

void Level::InitCameraSprite()
{
	camera_texture_ = new gef::TextureVita();
	camera_sprite_ = new gef::Sprite();
	camera_sprite_->set_position(gef::Vector4((float)DISPLAY_WIDTH / 2, (float)DISPLAY_HEIGHT / 2, 1));
	camera_sprite_->set_width((float)DISPLAY_WIDTH);
	camera_sprite_->set_height((float)DISPLAY_HEIGHT * VIEW_SCALE_V);
}

void Level::InitSprites()
{
	power_meter_texture_ = CreateTextureFromPNG("UI/PowerSlider.png", platform_);
	power_meter_sprite_.set_texture(power_meter_texture_);
	power_meter_sprite_.set_width(DISPLAY_WIDTH);
	power_meter_sprite_.set_height(DISPLAY_HEIGHT);
	power_meter_sprite_.set_position(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0);

	power_meter_notch_texture_ = CreateTextureFromPNG("UI/PowerSliderNotch.png", platform_);
	power_meter_notch_sprite_.set_texture(power_meter_notch_texture_);
	power_meter_notch_sprite_.set_width(DISPLAY_WIDTH);
	power_meter_notch_sprite_.set_height(DISPLAY_HEIGHT);
	power_meter_notch_sprite_.set_position(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0);

	pause_menu_texture_ = CreateTextureFromPNG("UI/PauseSprite.png", platform_);
	pause_menu_sprite_.set_texture(pause_menu_texture_);
	pause_menu_sprite_.set_width(DISPLAY_WIDTH);
	pause_menu_sprite_.set_height(DISPLAY_HEIGHT);
	pause_menu_sprite_.set_position(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0);

	loading_texture_ = CreateTextureFromPNG("UI/LoadingSprite.png", platform_);
	loading_sprite_.set_texture(loading_texture_);
	loading_sprite_.set_width(DISPLAY_WIDTH);
	loading_sprite_.set_height(DISPLAY_HEIGHT);
	loading_sprite_.set_position(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0);
}

void Level::InitAudio()
{
	if (!audio_manager_)
		return;
}

void Level::onExit()
{
	CleanUp();
}

void Level::CleanUp()
{
	delete primitive_builder_;
	primitive_builder_ = NULL;

	smartRelease();
	sampleRelease();

	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete cannon;
	cannon = NULL;

	delete goal;
	goal = NULL;

	delete bounce_platform1;
	bounce_platform1 = NULL;

	delete bounce_platform2;
	bounce_platform2 = NULL;

	// free up audio assets
	if (audio_manager_)
	{
		audio_manager_->UnloadAllSamples();
		audio_manager_->UnloadMusic();
	}

	delete audio_manager_;
	audio_manager_ = NULL;

}

void Level::PlaySFX(int sfx)
{
	if (!audio_manager_)
		return;

	audio_manager_->PlaySample(sfx);
}


void Level::PlaySFX(int sfx, float volume, float pitch)
{
	if (!audio_manager_)
		return;

	gef::VolumeInfo volumeInfo;
	volumeInfo.volume = volume;

	audio_manager_->SetSamplePitch(sfx, pitch);

	audio_manager_->SetSampleVoiceVolumeInfo(sfx, volumeInfo);
	audio_manager_->PlaySample(sfx);
}

baseState* Level::onUpdate(float frame_time)
{
	player_input_->Update();

	// Pause the game if the player presses the pause button
	if (player_input_->IsInputPressed("Pause") && !game_over_)
	{
		paused_ = !paused_;
	}

	// Return to the menu if the player pauses and presses continue
	if (paused_ && player_input_->IsInputPressed("Continue"))
	{
		return_to_menu_ = true;
		return new Menu(platform_);
	}

	// If the timer has hit 0, end the game
	if (timer_ <= 0)
	{
		EndGame();

		if (return_to_menu_ == true)
		{
			return new Menu(platform_);
		}

		return this;
	}

	if (paused_)
		return this;

	fps_ = 1.0f / frame_time;

	AppData* dat = sampleUpdateBegin();

	// use the tracking library to try and find markers
	smartUpdate(dat->currentImage);

	SetPowerMeterNotchPosition();
	CalculateCannonGoalDistance();

	// Updates for corresponding GameObjects are called within these functions
	CheckForMarker(0, cannon, frame_time);
	CheckForMarker(1, bounce_platform1, frame_time);
	CheckForMarker(2, bounce_platform2, frame_time);
	CheckForMarker(3, goal, frame_time);

	// Begin counting down timer after the fire shot is fired
	if(cannon->first_shot_fired_)
		timer_ -= frame_time;

	sampleUpdateEnd(dat);

	return this;
}

void Level::EndGame()
{
	game_over_ = true;

	// Return to menu if the player presses the Continue button
	if (player_input_->IsInputPressed("Continue")) 
	{
		return_to_menu_ = true;
	}
}

void Level::CalculateCannonGoalDistance()
{
	// Get world position of cannon and goal
	gef::Vector4 cannonPos = cannon->cannon_base_->transform_.GetWorldTransform().GetTranslation();
	gef::Vector4 goalPos = goal->transform_.GetWorldTransform().GetTranslation();

	// calculate a single float distance between the two - this is used to determine if the cannon is too near the goal
	cannon_goal_distance_ = sqrt(pow(goalPos.x() - cannonPos.x(), 2) + pow(goalPos.y() - cannonPos.y(), 2));
}

void Level::SetPowerMeterNotchPosition()
{
	// Move the "notch" on the power meter to match the value in code
	power_meter_notch_sprite_.set_position((float)DISPLAY_WIDTH / 2 + ((cannon->cannon_charge_power_ - 2.5f) * 57.5f), (float)DISPLAY_HEIGHT / 2, 0);
}

void Level::CheckForMarker(int markerNum, GameObject* object, float frame_time)
{
	if (sampleIsMarkerFound(markerNum))
	{
		// Update the position of the object based on the marker's transform
		UpdateMarker(object->marker_transform_, markerNum);

		// Render the object if the marker is visible
		object->render_ = true;

		object->Update(frame_time);
	}
	else
		object->render_ = false;
}

void Level::CheckForMarker(int markerNum, BouncePlatform* object, float frame_time)
{
	if (sampleIsMarkerFound(markerNum))
	{
		// Update the position of the object based on the marker's transform
		UpdateMarker(object->marker_transform_, markerNum);

		// Render the object if the marker is visible
		object->render_ = true;

		object->Update(frame_time);
	}
	else
		object->render_ = false;
}

void Level::CheckForMarker(int markerNum, Cannon* object, float frame_time)
{
	if (sampleIsMarkerFound(markerNum))
	{
		// Update the position of the object based on the marker's transform
		UpdateMarker(object->marker_transform_, markerNum);

		object->CollisionDetection(*bounce_platform1);
		object->CollisionDetection(*bounce_platform2);

		// If the cannonball collides with the goal
		int new_score_ = object->CollisionDetection(*goal);

		if(new_score_ > 0)
		{
			NextStage(new_score_);
		}
		
		// Render the object if the marker is visible
		object->render_ = true;

		object->Update(player_input_, frame_time);

		DetermineCannonGoalDistance();
	}
	else
		object->render_ = false;
}

void Level::DetermineCannonGoalDistance()
{
	if (cannon_goal_distance_ < cannon_goal_desired_distance_)
		cannon->far_from_goal_ = false;
	else
		cannon->far_from_goal_ = true;
}

void Level::NextStage(int new_score_)
{
	// Increase the timer by 5 and increment the score
	timer_ += 5;
	score_ += new_score_;

	// Randomize the scale for objects
	goal->RandomizeScale(0.0175f);
	bounce_platform1->RandomizeScale();
	bounce_platform2->RandomizeScale();

	// Randomize the offset from the origin of objects
	goal->RandomizeOffset();
	bounce_platform1->RandomizeOffset();
	bounce_platform2->RandomizeOffset();
}


void Level::UpdateMarker(MyTransform* markerTransform, int cardNumber)
{
	gef::Matrix44 marker_transform_matrix;
	sampleGetTransform(cardNumber, &marker_transform_matrix);
	markerTransform->Set(marker_transform_matrix);
}


void Level::HandlePlayerInput(float frame_time)
{
	player_input_->Update();
}

void Level::onRender()
{
	AppData* dat = sampleRenderBegin();
	gef::Matrix44 cameraMatrix;

	cameraMatrix = platform_.OrthographicFrustum(-1, 1, -1, 1, -1, 1);
	renderer_3d_->set_projection_matrix(cameraMatrix);

	sprite_renderer_->Begin(true);

	if (dat->currentImage)
	{
		camera_texture_->set_texture(dat->currentImage->tex_yuv);
		camera_sprite_->set_texture(camera_texture_);
		sprite_renderer_->DrawSprite(*camera_sprite_);
	}

	//DrawCameraOnScreen(dat);

	sprite_renderer_->End();

	CreateARMatrices();

	// Begin rendering 3D meshes, don't clear the frame buffer
	renderer_3d_->Begin(false);

	RenderGameObjects();

	renderer_3d_->End();

	RenderOverlay();

	sampleRenderEnd();
}

void Level::RenderGameObjects()
{
	if (bounce_platform1->render_)
		renderer_3d_->DrawMesh(*bounce_platform1);

	if (bounce_platform2->render_)
		renderer_3d_->DrawMesh(*bounce_platform2);

	if (goal->render_)
		renderer_3d_->DrawMesh(*goal);

	if (cannon->render_)
	{
		renderer_3d_->DrawMesh(*cannon->cannon_base_);
		renderer_3d_->DrawMesh(*cannon->cannon_launcher_);
		renderer_3d_->DrawMesh(*cannon->cannon_ball_);
	}
}

void Level::CreateARMatrices()
{
	gef::Matrix44 projectionMatrix;

	projectionMatrix = platform_.PerspectiveProjectionFov(SCE_SMART_IMAGE_FOV, (float)CAMERA_WIDTH / (float)CAMERA_HEIGHT, 0.05f, 100.0f);

	gef::Matrix44 scaleMatrix;
	scaleMatrix.Scale(gef::Vector4(1, VIEW_SCALE_V, 1));

	gef::Matrix44 finalProjectionMatrix = projectionMatrix * scaleMatrix;
	renderer_3d_->set_projection_matrix(finalProjectionMatrix);

	gef::Matrix44 viewMatrix;
	viewMatrix.SetIdentity();
	renderer_3d_->set_view_matrix(viewMatrix);
}

void Level::RenderOverlay()
{
	gef::Matrix44 proj_matrix2d;

	proj_matrix2d = platform_.OrthographicFrustum(0.0f, platform_.width(), 0.0f, platform_.height(), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(proj_matrix2d);
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}


void Level::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void Level::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void Level::DrawPowerMeter()
{
	sprite_renderer_->DrawSprite(power_meter_sprite_);
	sprite_renderer_->DrawSprite(power_meter_notch_sprite_);

	// Determine the opacity of the UI element depending on whether the cannon is far enough from the goal
	if (cannon_goal_distance_ < cannon_goal_desired_distance_)
	{
		power_meter_sprite_.set_colour(0x55999999);
		power_meter_notch_sprite_.set_colour(0x55999999);
	}
	else
	{
		power_meter_sprite_.set_colour(0xFFFFFFFF);
		power_meter_notch_sprite_.set_colour(0xFFFFFFFF);
	}
}

void Level::DrawGameOverScreen()
{
	font_->RenderText(sprite_renderer_, gef::Vector4((float)DISPLAY_WIDTH / 2, (float)DISPLAY_HEIGHT / 2 - 150, -0.9f), 3.0f, 0xffffffff, gef::TJ_CENTRE, "Game Over");
	font_->RenderText(sprite_renderer_, gef::Vector4((float)DISPLAY_WIDTH / 2, (float)DISPLAY_HEIGHT / 2 - 50, -0.9f), 2.5f, 0xffffffff, gef::TJ_CENTRE, "Your score was %i", score_);
	font_->RenderText(sprite_renderer_, gef::Vector4((float)DISPLAY_WIDTH / 2, (float)DISPLAY_HEIGHT / 2 + 50, -0.9f), 1.5f, 0xffffffff, gef::TJ_CENTRE, "Press X to return to menu");
}

void Level::DrawTimer()
{
	// Determine to display the timer in white or red depending on how much time is left.
	if (timer_ > 5)
		font_->RenderText(sprite_renderer_, gef::Vector4(775.0f, 0.0f, -0.9f), 1.5f, 0xffffffff, gef::TJ_LEFT, "Timer: %.1f", abs(timer_));
	else
		font_->RenderText(sprite_renderer_, gef::Vector4(775.0f, 0.0f, -0.9f), 1.5f, 0xff0000ff, gef::TJ_LEFT, "Timer: %.1f", abs(timer_));
}

void Level::DrawDistanceWarning()
{
	if (cannon_goal_distance_ < cannon_goal_desired_distance_)
		font_->RenderText(sprite_renderer_, gef::Vector4(25.0f, 0.0f, -0.9f), 1.0f, 0xff0000ff, gef::TJ_LEFT, "Cannon is too near to the goal!");
}

void Level::DrawMenuOverlays()
{
	if (return_to_menu_)
	{
		sprite_renderer_->DrawSprite(loading_sprite_);
	}
	else
	{
		if (paused_)
		{
			sprite_renderer_->DrawSprite(pause_menu_sprite_);
		}
	}
}

void Level::DrawHUD()
{
	// Draw the power meter that represents the cannon's charge power
	DrawPowerMeter();

	if (font_)
	{
		if (game_over_ && !return_to_menu_)
		{
			DrawGameOverScreen();
			return;
		}

		// Draw the score
		font_->RenderText(sprite_renderer_, gef::Vector4(25.0f, 440.0f, -0.9f), 1.5f, 0xffffffff, gef::TJ_LEFT, "Score: %i", score_);

		DrawTimer();

		DrawDistanceWarning();
	}

	// Determine if the pause or game over menus need to be drawn
	DrawMenuOverlays();
}

void Level::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, 300.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}
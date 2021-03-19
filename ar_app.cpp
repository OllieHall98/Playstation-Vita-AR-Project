//#include "ar_app.h"
//#include <system/platform.h>
//#include <graphics/sprite_renderer.h>
//#include <graphics/texture.h>
//#include <graphics/mesh.h>
//#include <graphics/primitive.h>
//#include <assets/png_loader.h>
//#include <graphics/image_data.h>
//#include <graphics/font.h>
//#include <input/touch_input_manager.h>
//#include <maths/vector2.h>
//#include <input/input_manager.h>
//#include <maths/math_utils.h>
//#include <graphics/renderer_3d.h>
//#include <graphics/render_target.h>
//#include <graphics/scene.h>
//#include <sony_sample_framework.h>
//#include <sony_tracking.h>
//#include "loadTexture.h"
#include "ar_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/input_manager.h>
#include <input\sony_controller_input_manager.h>
#include <input\keyboard.h>
#include "states.h"
#include "Level.h"
#include "Menu.h"

ARApp::ARApp(gef::Platform& platform) :
	Application(platform)
{
}

void ARApp::Init()
{
	spriteRenderer = gef::SpriteRenderer::Create(platform_);
	renderer3d = gef::Renderer3D::Create(platform_); // create the renderer for draw 3D geometry
	primitiveBuilder = new PrimitiveBuilder(platform_); // initialise primitive builder to make create some 3D geometry easier

	currentState = new Menu(platform_);
	nextState = currentState;
	currentState->onEnter();
}

void ARApp::CleanUp()
{
	delete primitiveBuilder;
	primitiveBuilder = NULL;

	delete renderer3d;
	renderer3d = NULL;

	delete spriteRenderer;
	spriteRenderer = NULL;
}

bool ARApp::Update(float frameTime)
{
	if (nextState != currentState)
	{
		currentState->onExit();
		delete currentState;
		currentState = nextState;
		currentState->onEnter();
	}

	nextState = currentState->onUpdate(frameTime);

	return true;
}

void ARApp::Render()
{
	currentState->onRender();
}

void ARApp::initialiseFont()
{
	font = new gef::Font(platform_);
	font->Load("comic_sans");
}



//ARApp::ARApp(gef::Platform& platform) :
//	Application(platform),
//	//input_manager_(NULL),
//	sprite_renderer_(NULL),
//	font_(NULL),
//	renderer_3d_(NULL),
//	model_scene_(NULL),
//	primitive_builder_(NULL),
//	cannon(NULL)
//	//platform1(NULL),
//	//platform1MarkerTransform_(NULL),
//	//goal(NULL),
//	//goalTransform_(NULL)
//{
//}
//
//void ARApp::Init()
//{
//	//input_manager_ = gef::InputManager::Create(platform_);
//	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
//	renderer_3d_ = gef::Renderer3D::Create(platform_);
//	primitive_builder_ = new PrimitiveBuilder(platform_);
//	player_input_ = new GlobalInput(platform_);
//
//	InitFont();
//	SetupLights();
//
//	camera_vertical_scale_ = (960 / 544) / (640 / 480);
//
//	// initialise sony framework
//	sampleInitialize();
//	smartInitialize();
//
//	// reset marker tracking
//	AppData* dat = sampleUpdateBegin();
//	smartTrackingReset();
//	sampleUpdateEnd(dat);
//
//	camera_texture_ = new gef::TextureVita();
//
//	camera_sprite_ = new gef::Sprite();
//	camera_sprite_->set_position(gef::Vector4((float)DISPLAY_WIDTH / 2, (float)DISPLAY_HEIGHT / 2, 1));
//	camera_sprite_->set_width((float)DISPLAY_WIDTH);
//	camera_sprite_->set_height((float)DISPLAY_HEIGHT * VIEW_SCALE_V);
//
//	cannon = new Cannon(platform_);
//	bounce_platform1 = new BouncePlatform(platform_);
//	bounce_platform2 = new BouncePlatform(platform_);
//	goal = new Goal(platform_);
//
//	cannon->goal_ = goal;
//
//	// SPRITES
//
//	power_meter_texture = CreateTextureFromPNG("UI/PowerSlider.png", platform_);
//	power_meter_sprite.set_texture(power_meter_texture);
//	power_meter_sprite.set_width(DISPLAY_WIDTH);
//	power_meter_sprite.set_height(DISPLAY_HEIGHT);
//	power_meter_sprite.set_position(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0);
//
//	power_meter_notch_texture = CreateTextureFromPNG("UI/PowerSliderNotch.png", platform_);
//	power_meter_notch_sprite.set_texture(power_meter_notch_texture);
//	power_meter_notch_sprite.set_width(DISPLAY_WIDTH);
//	power_meter_notch_sprite.set_height(DISPLAY_HEIGHT);
//	power_meter_notch_sprite.set_position(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0);
//}
//
//void ARApp::CleanUp()
//{
//	delete primitive_builder_;
//	primitive_builder_ = NULL;
//
//	smartRelease();
//	sampleRelease();
//
//	CleanUpFont();
//	delete sprite_renderer_;
//	sprite_renderer_ = NULL;
//
//	delete renderer_3d_;
//	renderer_3d_ = NULL;
//
//	delete cannon;
//	cannon = NULL;
//
//	delete goal;
//	goal = NULL;
//
//	delete bounce_platform1;
//	bounce_platform1 = NULL;
//
//	delete bounce_platform2;
//	bounce_platform2 = NULL;
//
//}
//
//bool ARApp::Update(float frame_time)
//{
//	fps_ = 1.0f / frame_time;
//
//	AppData* dat = sampleUpdateBegin();
//
//	// use the tracking library to try and find markers
//	smartUpdate(dat->currentImage);
//
//	UpdateUI();
//	CalculateCannonGoalDistance();
//
//	CheckForMarker(0, cannon, frame_time);
//	CheckForMarker(1, bounce_platform1, frame_time);
//	CheckForMarker(2, bounce_platform2, frame_time);
//	CheckForMarker(3, goal, frame_time);
//
//	sampleUpdateEnd(dat);
//
//	return true;
//}
//
//void ARApp::CalculateCannonGoalDistance()
//{
//	gef::Vector4 cannonPos = cannon->cannon_base_->transform.GetWorldTransform().GetTranslation();
//	gef::Vector4 goalPos = goal->transform.GetWorldTransform().GetTranslation();
//
//	cannon_goal_distance = sqrt(pow(goalPos.x() - cannonPos.x(), 2) + pow(goalPos.y() - cannonPos.y(), 2));
//}
//
//void ARApp::UpdateUI()
//{
//	power_meter_notch_sprite.set_position((float)DISPLAY_WIDTH / 2 + ((cannon->cannon_charge_power - 2.5f) * 57.5f), (float)DISPLAY_HEIGHT / 2, 0);
//}
//
//void ARApp::CheckForMarker(int markerNum, GameObject* object, float frame_time)
//{
//	if (sampleIsMarkerFound(markerNum))
//	{
//		UpdateMarker(object->marker_transform_, markerNum);
//
//		object->render = true;
//		object->Update(frame_time);
//	}
//	else
//		object->render = false;
//}
//
//void ARApp::CheckForMarker(int markerNum, BouncePlatform* object, float frame_time)
//{
//	if (sampleIsMarkerFound(markerNum))
//	{
//		UpdateMarker(object->marker_transform_, markerNum);
//
//		object->render = true;
//		object->Update(frame_time);
//	}
//	else
//		object->render = false;
//}
//
//void ARApp::CheckForMarker(int markerNum, Cannon* object, float frame_time)
//{
//	if (sampleIsMarkerFound(markerNum))
//	{
//		UpdateMarker(object->marker_transform_, markerNum);
//
//		object->CollisionDetection(*bounce_platform1);
//		object->CollisionDetection(*bounce_platform2);
//		object->CollisionDetection(*goal);
//
//		object->render = true;
//
//		object->Update(player_input_, frame_time);
//
//		if (cannon_goal_distance < cannon_goal_desired_distance)
//			cannon->canControl = false;
//		else
//			cannon->canControl = true;
//	}
//	else
//		object->render = false;
//}
//
//
//void ARApp::UpdateMarker(MyTransform* markerTransform, int cardNumber)
//{
//	gef::Matrix44 marker_transform_matrix;
//	sampleGetTransform(cardNumber, &marker_transform_matrix);
//	markerTransform->Set(marker_transform_matrix);
//}
//
//
//void ARApp::HandlePlayerInput(float frame_time)
//{
//	player_input_->Update();
//}
//
//void ARApp::Render()
//{
//	AppData* dat = sampleRenderBegin();
//
//	gef::Matrix44 cameraMatrix;
//
//	cameraMatrix = platform_.OrthographicFrustum(-1, 1, -1, 1, -1, 1);
//	renderer_3d_->set_projection_matrix(cameraMatrix);
//
//	sprite_renderer_->Begin(true);
//
//	if (dat->currentImage)
//	{
//		camera_texture_->set_texture(dat->currentImage->tex_yuv);
//		camera_sprite_->set_texture(camera_texture_);
//		sprite_renderer_->DrawSprite(*camera_sprite_);
//	}
//
//	sprite_renderer_->End();
//
//	gef::Matrix44 projectionMatrix;
//
//	projectionMatrix = platform_.PerspectiveProjectionFov(SCE_SMART_IMAGE_FOV, (float)CAMERA_WIDTH / (float)CAMERA_HEIGHT, 0.05f, 100.0f);
//
//	gef::Matrix44 scaleMatrix;
//	scaleMatrix.Scale(gef::Vector4(1, VIEW_SCALE_V, 1));
//
//	gef::Matrix44 finalProjectionMatrix = projectionMatrix * scaleMatrix;
//	renderer_3d_->set_projection_matrix(finalProjectionMatrix);
//
//	gef::Matrix44 viewMatrix;
//	viewMatrix.SetIdentity();
//	renderer_3d_->set_view_matrix(viewMatrix);
//
//	// Begin rendering 3D meshes, don't clear the frame buffer
//	renderer_3d_->Begin(false);
//
//	if(bounce_platform1->render)
//		renderer_3d_->DrawMesh(*bounce_platform1);
//
//	if (bounce_platform2->render)
//		renderer_3d_->DrawMesh(*bounce_platform2);
//
//	if(goal->render) 
//		renderer_3d_->DrawMesh(*goal);
//
//	if (cannon->render)
//	{
//		renderer_3d_->DrawMesh(*cannon->cannon_base_);
//		renderer_3d_->DrawMesh(*cannon->cannon_launcher_);
//		renderer_3d_->DrawMesh(*cannon->cannon_ball_);
//		//renderer_3d_->DrawMesh(*cannon->cannon_ball_shadow);
//	}
//
//	renderer_3d_->End();
//
//	RenderOverlay();
//
//	sampleRenderEnd();
//}
//
//
//void ARApp::RenderOverlay()
//{
//	//
//	// render 2d hud on top
//	//
//	gef::Matrix44 proj_matrix2d;
//
//	proj_matrix2d = platform_.OrthographicFrustum(0.0f, platform_.width(), 0.0f, platform_.height(), -1.0f, 1.0f);
//	sprite_renderer_->set_projection_matrix(proj_matrix2d);
//	sprite_renderer_->Begin(false);
//	DrawHUD();
//	sprite_renderer_->End();
//}
//
//
//void ARApp::InitFont()
//{
//	font_ = new gef::Font(platform_);
//	font_->Load("comic_sans");
//}
//
//void ARApp::CleanUpFont()
//{
//	delete font_;
//	font_ = NULL;
//}
//
//void ARApp::DrawHUD()
//{
//	sprite_renderer_->DrawSprite(power_meter_sprite);
//	sprite_renderer_->DrawSprite(power_meter_notch_sprite);
//
//	if (cannon_goal_distance < cannon_goal_desired_distance)
//	{
//		power_meter_sprite.set_colour(0x55999999);
//		power_meter_notch_sprite.set_colour(0x55999999);
//	}
//	else
//	{
//		power_meter_sprite.set_colour(0xFFFFFFFF);
//		power_meter_notch_sprite.set_colour(0xFFFFFFFF);
//	}
//
//	if(font_)
//	{
//		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
//		font_->RenderText(sprite_renderer_, gef::Vector4(25.0f, 440.0f, -0.9f), 1.5f, 0xffffffff, gef::TJ_LEFT, "Score: %.1f", cannon->score);
//
//		if(cannon_goal_distance < cannon_goal_desired_distance)
//			font_->RenderText(sprite_renderer_, gef::Vector4(25.0f, 0.0f, -0.9f), 1.0f, 0xff0000ff, gef::TJ_LEFT, "Cannon is too near to the goal!");
//	}
//}
//
//void ARApp::SetupLights()
//{
//	gef::PointLight default_point_light;
//	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
//	default_point_light.set_position(gef::Vector4(-300.0f, 300.0f, 100.0f));
//
//	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
//	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
//	default_shader_data.AddPointLight(default_point_light);
//}
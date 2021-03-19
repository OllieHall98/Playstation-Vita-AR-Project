#include "Menu.h"
#include <input\sony_controller_input_manager.h>
#include <system/platform.h>
#include "loadTexture.h"
#include <sony_sample_framework.h>
#include "Level.h"

Menu::Menu(gef::Platform& platform) :
	platform_(platform),
	sprite_renderer_(NULL),
	player_input_(NULL),
	font_(NULL)
{
}

Menu::~Menu()
{
}

void Menu::onEnter()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	input_ = gef::InputManager::Create(platform_);
	player_input_ = new GlobalInput(platform_);

	initialiseFont();

	InitMenuSprite();
	InitLoadingSprite();

	loading_ = false;
}

void Menu::InitMenuSprite()
{
	menu_texture_ = CreateTextureFromPNG("UI/MenuSprite.png", platform_);
	menu_sprite_.set_texture(menu_texture_);
	menu_sprite_.set_width(DISPLAY_WIDTH);
	menu_sprite_.set_height(DISPLAY_HEIGHT);
	menu_sprite_.set_position(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0);
}

void Menu::InitLoadingSprite()
{
	loading_texture_ = CreateTextureFromPNG("UI/LoadingSprite.png", platform_);
	loading_sprite_.set_texture(loading_texture_);
	loading_sprite_.set_width(DISPLAY_WIDTH);
	loading_sprite_.set_height(DISPLAY_HEIGHT);
	loading_sprite_.set_position(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0);
}

void Menu::onExit()
{
	cleanUp();
}

void Menu::initialiseFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

baseState * Menu::onUpdate(float frameTime)
{
	player_input_->Update();

	// Load the game if the player presses the Continue button
	if (player_input_->IsInputPressed("Continue"))
	{
		loading_ = true;
		return new Level(platform_);
	}

	return this;
}

void Menu::onRender()
{
	if (sprite_renderer_)
	{
		sprite_renderer_->Begin(false);
		
		if (loading_)
			sprite_renderer_->DrawSprite(loading_sprite_);
		else
			sprite_renderer_->DrawSprite(menu_sprite_);

		sprite_renderer_->End();
	}


}

void Menu::cleanUp()
{
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete font_;
	font_ = NULL;

	delete player_input_;
	player_input_ = NULL;
}
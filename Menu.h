#pragma once
#include "input\input_manager.h"
#include "graphics\sprite_renderer.h"
#include <audio/audio_manager.h>
#include "graphics\font.h"
#include "graphics\sprite.h"
#include "GlobalInput.h"
#include "states.h"

class Menu : public baseState
{
public:
	void onEnter() override;
	void InitMenuSprite();
	void InitLoadingSprite();
	void onExit() override;
	baseState* onUpdate(float frameTime) override;
	void onRender() override;

	Menu(gef::Platform& platform);
	~Menu();

private:
	void cleanUp();
	void initialiseFont();

	gef::Platform& platform_;
	gef::InputManager* input_;
	gef::SpriteRenderer* sprite_renderer_;
	GlobalInput* player_input_;
	gef::Font* font_;

	gef::Sprite menu_sprite_;
	gef::Texture* menu_texture_;

	gef::Sprite loading_sprite_;
	gef::Texture* loading_texture_;

	bool loading_ = false;
};
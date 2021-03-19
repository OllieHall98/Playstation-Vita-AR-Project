#include "GlobalInput.h"
#include <algorithm>
#include <vector>
#include "system/debug_log.h"

// Made with help from Ben Slaney

GlobalInput::GlobalInput(gef::Platform& platform) :
	platform_(platform)
{
	Init(platform_);
}

// CHANGE TO ENUMS

std::vector<Input> const input_table_ =
{
	{"Up",			gef::Keyboard::KC_W,		gef_SONY_CTRL_UP	},
	{"Down",		gef::Keyboard::KC_S,		gef_SONY_CTRL_DOWN	},
	{"Left",		gef::Keyboard::KC_A,		gef_SONY_CTRL_LEFT	},
	{"Right",		gef::Keyboard::KC_D,		gef_SONY_CTRL_RIGHT	},
	{"Fire",		gef::Keyboard::KC_SPACE,	gef_SONY_CTRL_R2},
	{"Bounce",		gef::Keyboard::KC_LCONTROL,	gef_SONY_CTRL_L2},
	{"Continue",	gef::Keyboard::KC_F,		gef_SONY_CTRL_CROSS},
	{"Pause",		gef::Keyboard::KC_ESCAPE,	gef_SONY_CTRL_OPTIONS},
};

void GlobalInput::Init(gef::Platform& platform)
{
	input_manager_ = gef::InputManager::Create(platform);
	keyboard_ = input_manager_->keyboard();
	controller_manager_ = input_manager_->controller_input();
}

void GlobalInput::Update()
{
	input_manager_->Update();

}

bool GlobalInput::IsInputPressed(const char* input_name)
{
	if (!input_manager_) return false;

	// Retrieve corresponding input from table
	Input input = RetrieveInput(input_name);

	// if the keyboard is being used, return whether that key is currently pressed
	if (keyboard_) return keyboard_->IsKeyPressed(input.keyboard_input);

	// if the controller is being used, return whether that button is currently pressed
	if (controller_manager_)
	{
		const gef::SonyController* controller = controller_manager_->GetController(0);
		return controller->buttons_pressed() & input.vita_input;
	}

	return false;
}

bool GlobalInput::IsInputUp(const char* input_name)
{
	if (!input_manager_) return false;

	// Retrieve corresponding input from table
	Input input = RetrieveInput(input_name);

	// if the keyboard is being used, return whether that key is currently up
	if (keyboard_) return keyboard_->IsKeyReleased(input.keyboard_input);

	// if the controller is being used, return whether that button is currently up
	if (controller_manager_)
	{
		const gef::SonyController* controller = controller_manager_->GetController(0);
		return controller->buttons_released() & input.vita_input;
	}

	return false;
}

bool GlobalInput::IsInputDown(const char* input_name)
{
	if (!input_manager_) return false;

	// Retrieve corresponding input from table
	Input input = RetrieveInput(input_name);

	// if the keyboard is being used, return whether that key is currently down
	if (keyboard_) return keyboard_->IsKeyDown(input.keyboard_input);

	// if the controller is being used, return whether that button is currently down
	if (controller_manager_)
	{
		const gef::SonyController* controller = controller_manager_->GetController(0);
		return controller->buttons_down() & input.vita_input;
	}

	return false;
}

Input GlobalInput::RetrieveInput(const char* input_name)
{
	// This function searches the input table to find an input by name
	for (int i = 0; i < input_table_.size(); i++)
	{
		if (input_table_[i].input_name == input_name)
		{
			return input_table_[i];
		}
	}
}

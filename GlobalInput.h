#include <string>
#include "input/input_manager.h"
#include "gef.h"
#include "input\keyboard.h"
#include "input\sony_controller_input_manager.h"

struct Input
{
	// The label of the input (e.g. Jump)
	std::string input_name;

	// The assigned keyboard key for input
	gef::Keyboard::KeyCode keyboard_input;

	// The assigned vita button for input
	Int32 vita_input;
};

#pragma once
class GlobalInput
{
public:
	GlobalInput(gef::Platform& platform);

	void Init(gef::Platform& platform);
	void Update();

	bool IsInputDown(const char* input_name);
	bool IsInputUp(const char* input_name);
	bool IsInputPressed(const char* input_name);

private:
	Input RetrieveInput(const char* input_name);

	gef::Platform& platform_;
	gef::InputManager* input_manager_;
	gef::Keyboard* keyboard_;
	gef::SonyControllerInputManager* controller_manager_;
	const gef::SonyController* controller_;
};


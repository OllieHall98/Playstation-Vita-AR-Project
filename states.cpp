#include "states.h"

//void menuState::onEnter(gef::Platform& platform, gef::Font* font, gef::InputManager* input)
//{
//	menu = new Menu(platform);
//	state = MState::M_MENU;
//	menu->initialise();
//}

//void menuState::onExit()
//{
//	menu->cleanUp();
//}
//
//baseState* menuState::onUpdate(float frameTime)
//{
//	menu->update(frameTime);
//
//	switch (state)
//	{
//		case(M_MENU):
//		{
//			return new menuState;
//			break;
//		}
//		case(M_EXIT):
//		{
//			break;
//		}
//	}
//}

//void menuState::onRender()
//{
//	menu->render();
//}

void levelState::onEnter(gef::Platform& platform)
{
	level = new Level(platform);
	state = LevelState::LEVEL;
	level->Init();

}

void levelState::onExit()
{
	level->CleanUp();
}

baseState* levelState::onUpdate(float frameTime)
{
	level->Update(frameTime);

	switch (state)
	{
	case(MENU):
	{
		//return new menuState;
		break;
	}
	case(LEVEL):
	{
		break;
	}
	}
}

void levelState::onRender()
{
	level->Render();
}
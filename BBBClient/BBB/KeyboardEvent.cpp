#include "stdafx.h"
#include "Game.h"
#include "KeyboardEvent.h"

void KEY_BOARD_EVENT_MANAGER::KeyBoard(GLFWwindow* window, int key, int code, int action, int modifiers)
{
	key_events.emplace(key, code, action, modifiers, clk::now());
}

void KEY_BOARD_EVENT_MANAGER::ProcessInput()
{
	while (!key_events.empty())
	{
		key_event event = key_events.front();
		key_events.pop();
		if (key_functions.count(event.key))
		{
			key_functions[event.key]();
		}
		keys[event.key] = event.action;
		Game::get().player->process_input(event);
	}
}

#include "stdafx.h"
#include "DynamicObj.h"
#include "KeyboardEvent.h"



void ControllObj::process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key)
{
	bool pressed = (key.action != GLFW_RELEASE);
	switch (key.key)
	{
	case GLFW_KEY_W :
	{
		_up_on = pressed;
	}
	CASE GLFW_KEY_S :
	{
		_down_on = pressed;
	}
	CASE GLFW_KEY_A :
	{
		_left_on = pressed;
	}
	CASE GLFW_KEY_D :
	{
		_right_on = pressed;
	}
	CASE GLFW_KEY_SPACE :
	{
		_brake_on = pressed;
	}
	break; default: break;
	}
}

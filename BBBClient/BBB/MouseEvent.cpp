#include "stdafx.h"
#include "MouseEvent.h"

void MOUSE_EVENT_MANAGER::Scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	scroll_events_.emplace(xoffset, yoffset, clk::now());
}

void MOUSE_EVENT_MANAGER::MouseButton(GLFWwindow* window, int key, int action, int modifiers)
{
	button_events_.emplace(key, action, modifiers, clk::now());
}

void MOUSE_EVENT_MANAGER::CursorPosition(GLFWwindow* window, double xpos, double ypos)
{
	pos_events_.emplace(xpos, ypos, clk::now());
}

void MOUSE_EVENT_MANAGER::BindMainScrollFunc(main_scroll_func func)
{
	main_scroll_func_ = func;
}

void MOUSE_EVENT_MANAGER::BindMainButtonFunc(main_button_func func)
{
	main_button_func_ = func;
}

void MOUSE_EVENT_MANAGER::BindMainPosFunc(main_pos_func func)
{
	main_pos_func_ = func;
}

void MOUSE_EVENT_MANAGER::BindScrollFunc(background_scroll_func func)
{
	scroll_func_ = func;
}

void MOUSE_EVENT_MANAGER::BindButtonFunc(background_button_func func)
{
	button_func_ = func;
}

void MOUSE_EVENT_MANAGER::BindPosFunc(background_pos_func func)
{
	pos_func_ = func;
}

void MOUSE_EVENT_MANAGER::ProcessInput()
{
	while (!scroll_events_.empty())
	{
		scroll_event& event = scroll_events_.front(); scroll_events_.pop();

		if (main_scroll_func_(event))
		{
			continue;
		}

		scroll_func_(event);
	}

	while (!button_events_.empty())
	{
		button_event& event = button_events_.front(); button_events_.pop();

		switch (event.button)
		{
		case GLFW_MOUSE_BUTTON_LEFT: { L_click_ = event.action; } break;
		case GLFW_MOUSE_BUTTON_RIGHT: { R_click_ = event.action; } break;
		case GLFW_MOUSE_BUTTON_MIDDLE: { Wheel_click_ = event.action; } break;
		}

		if (main_button_func_(event))
		{
			continue;
		}

		button_func_(event);
	}

	while (!pos_events_.empty())
	{
		pos_event& event = pos_events_.front(); pos_events_.pop();

		if (main_pos_func_(event))
		{
			prev_x_ = event.xpos;
			prev_y_ = event.ypos;
			continue;
		}
		pos_func_(event);

		prev_x_ = event.xpos;
		prev_y_ = event.ypos;
	}
}

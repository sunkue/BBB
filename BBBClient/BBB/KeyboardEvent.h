#pragma once


struct KEY_BOARD_EVENT_MANAGER
{
	SINGLE_TON(KEY_BOARD_EVENT_MANAGER) = default;

public:
	using key = int;
	using action = int;

	struct key_event
	{
		int key, code, action, modifiers;
		clk::time_point time_of_event;
	};

	std::map<key, action> keys;
	std::queue<key_event> key_events;
	std::map<key, std::function<void()>> key_functions;

	void KeyBoard(GLFWwindow* window, int key, int code, int action, int modifiers);
	void BindKeyFunc(key key, std::function<void()> func)
	{
		key_functions[key] = func;
	}
	void ProcessInput();
};

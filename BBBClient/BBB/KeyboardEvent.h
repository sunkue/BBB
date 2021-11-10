#pragma once


class KEY_BOARD_EVENT_MANAGER
{
	SINGLE_TON(KEY_BOARD_EVENT_MANAGER) = default;

public:

	struct key_event
	{
		int key, code, action, modifiers;
		clk::time_point time_of_event;
	};
	
	using key = int;
	using action = int;
	// main_func return true if wanna don't call other keyfunctions with the key.
	using main_key_func = std::function<bool(const KEY_BOARD_EVENT_MANAGER::key_event&)>;
	using key_func = std::function<void()>;

private:
	std::map<key, action> keys;
	std::queue<key_event> key_events;
	std::map<key, key_func> key_functions;
	main_key_func main_func;

public:
	void KeyBoard(GLFWwindow* window, int key, int code, int action, int modifiers);
	void BindKeyFunc(key key, key_func func)
	{
		key_functions[key] = func;
	}

	void BindMainKeyFunc(main_key_func func)
	{
		main_func = func;
	}
	void ProcessInput();
};

#pragma once


class MOUSE_EVENT_MANAGER
{
	SINGLE_TON(MOUSE_EVENT_MANAGER) = default;

public:

	struct scroll_event
	{
		double xoffset, yoffset;
		clk::time_point time_of_event;
	};

	struct button_event
	{
		int button, action, modifiers;
		clk::time_point time_of_event;
	};

	struct pos_event
	{
		double xpos, ypos;
		clk::time_point time_of_event;
	};

	using main_scroll_func = std::function<bool(const scroll_event&)>;
	using background_scroll_func = std::function<void(const scroll_event&)>;

	using main_button_func = std::function<bool(const button_event&)>;
	using background_button_func = std::function<void(const button_event&)>;

	using main_pos_func = std::function<bool(const pos_event&)>;
	using background_pos_func = std::function<void(const pos_event&)>;

private:
	std::queue<scroll_event> scroll_events_;
	std::queue<button_event> button_events_;
	std::queue<pos_event> pos_events_;

private:
	background_scroll_func scroll_func_ = [](const auto&) { return; };
	background_button_func button_func_ = [](const auto&) { return; };
	background_pos_func pos_func_ = [](const auto&) { return; };

	main_scroll_func  main_scroll_func_ = [](const auto&) { return false; };
	main_button_func  main_button_func_ = [](const auto&) { return false; };
	main_pos_func main_pos_func_= [](const auto&) { return false; };

public:
	GET(L_click);
	GET(R_click);
	GET(Wheel_click);
	GET(prev_x);
	GET(prev_y);
	// for gui testing delete later.
	GET_REF_UNSAFE(L_click);
	GET_REF_UNSAFE(R_click);
	GET_REF_UNSAFE(Wheel_click);
	GET_REF_UNSAFE(prev_x);
	GET_REF_UNSAFE(prev_y);
private:
	bool L_click_ = false;
	bool R_click_ = false;
	bool Wheel_click_ = false;

	double prev_x_;
	double prev_y_;

public:
	void Scroll(GLFWwindow* window, double xoffset, double yoffset);
	void MouseButton(GLFWwindow* window, int key, int action, int modifiers);
	void CursorPosition(GLFWwindow* window, double xpos, double ypos);

	void BindMainScrollFunc(main_scroll_func func);
	void BindMainButtonFunc(main_button_func func);
	void BindMainPosFunc(main_pos_func func);

	void BindScrollFunc(background_scroll_func func);
	void BindButtonFunc(background_button_func func);
	void BindPosFunc(background_pos_func func);

	void ProcessInput();
};


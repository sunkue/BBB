
#include "stdafx.h"
#include "Renderer.h"
#include "Obj.h"
#include "Camera.h"
#include "Game.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"

;

void DoNextFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	gui::NewFrame();

	KEY_BOARD_EVENT_MANAGER::get().ProcessInput();
	MOUSE_EVENT_MANAGER::get().ProcessInput();

	Game::get().update();

	gui::Begin("SUNKUE_ENGINE");
	gui::Text("MOUSE_BUTTON_CHECK");
	gui::Checkbox("L", &MOUSE_EVENT_MANAGER::get().get_L_click());
	gui::Checkbox("R", &MOUSE_EVENT_MANAGER::get().get_R_click());
	gui::Checkbox("Wheel", &MOUSE_EVENT_MANAGER::get().get_Wheel_click());
	gui::Text("MOUSE_POS_CHECK");
	float x = MOUSE_EVENT_MANAGER::get().get_prev_x();
	float y = MOUSE_EVENT_MANAGER::get().get_prev_y();
	gui::InputFloat("x", &x);
	gui::InputFloat("y", &y);
	gui::End();

	Renderer::get().draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Renderer::get().get_player()->draw_gui();
	Renderer::get().get_main_camera()->draw_gui();

	if (gui::GetIO().WantCaptureMouse)
	{
		//cout << "!" << endl;
	}
	else
	{
		//cout << "?" << endl;
	}

	// gui texture buffers
	const float gui_tex_buffer_ratio = 0.1f;
	auto gui_texture_size = ImVec2(screen.width * gui_tex_buffer_ratio, screen.height * gui_tex_buffer_ratio);

	auto depthbuffer = Renderer::get().get_depth_renderer()->depthmap_tbo->id;
	gui::Begin("depthbuffer(shadow)");
	gui::Image((void*)depthbuffer, gui_texture_size, ImVec2(0, 1), ImVec2(1, 0));
	gui::End();

	auto renderbuffer = Renderer::get().get_screen_renderer()->screen_tbo->id;
	gui::Begin("renderbuffer(predraw)");
	gui::Image((void*)renderbuffer, gui_texture_size, ImVec2(0, 1), ImVec2(1, 0));
	gui::End();

	// gui::ShowMetricsWindow();

	gui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(gui::GetDrawData());

	glfwPollEvents();
	auto fps = 1000 / (GAME_SYSTEM::get().tick_time().count() + 1);
	string title = "("s + to_string(fps) + " fps)"s;
	glfwSetWindowTitle(Game::get().window, title.c_str());

	glfwSwapBuffers(Game::get().window);
}




void MouseWheel(const MOUSE_EVENT_MANAGER::scroll_event& scroll)
{
	auto camera = Renderer::get().get_main_camera();
	if (scroll.yoffset > 0)
	{
		auto diff = camera->get_diff();
		camera->set_diff(diff * 0.875f);
		//- 1.31538 0.789227 0
		//- 1.4798 0.88788 0
	}
	else
	{
		auto diff = camera->get_diff();
		camera->set_diff(diff * 1.125f);
	}
}

void BindDefaultInputFuncs()
{
	KEY_BOARD_EVENT_MANAGER::get().BindKeyFunc(GLFW_KEY_LEFT_SHIFT,
		[](auto&) { Game::get().renderer.get_main_camera()->camera_shake(0.2f); });

	KEY_BOARD_EVENT_MANAGER::get().BindKeyFunc(GLFW_KEY_ESCAPE,
		[](const auto& key) { if (key.action != GLFW_RELEASE) Game::get().renderer.swap_player_ghost(); });

	MOUSE_EVENT_MANAGER::get().BindScrollFunc(
		[](const auto& scroll) { MouseWheel(scroll); });

}




int main()
{
	GLFWwindow* window;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(screen.width, screen.height, "SUNKUE", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow* window, int w, int h)
		{ Renderer::get().reshape(w, h); DoNextFrame(); });
	glfwSetScrollCallback(window,
		[](GLFWwindow* window, double xoffset, double yoffset)
		{ MOUSE_EVENT_MANAGER::get().Scroll(window, xoffset, yoffset);  });
	glfwSetMouseButtonCallback(window,
		[](GLFWwindow* window, int key, int action, int modifiers)
		{ MOUSE_EVENT_MANAGER::get().MouseButton(window, key, action, modifiers);  });
	glfwSetCursorPosCallback(window,
		[](GLFWwindow* window, double xpos, double ypos)
		{ MOUSE_EVENT_MANAGER::get().CursorPosition(window, xpos, ypos);  });
	glfwSetKeyCallback(window,
		[](GLFWwindow* window, int key, int code, int action, int modifiers)
		{ KEY_BOARD_EVENT_MANAGER::get().KeyBoard(window, key, code, action, modifiers);  });

	IMGUI_CHECKVERSION();
	gui::CreateContext();

	ImGuiIO& io = gui::GetIO();

	gui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

	Game::get().window = window;
	Renderer::get().reshape(screen.width, screen.height);

	BindDefaultInputFuncs();

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		DoNextFrame();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	gui::DestroyContext();

	glfwTerminate();
}


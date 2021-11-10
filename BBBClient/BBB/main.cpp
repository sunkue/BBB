
#include "stdafx.h"
#include "Renderer.h"
#include "VAO_OBJ.h"
#include "Camera.h"
#include "Game.h"
#include "KeyboardEvent.h"

GLFWwindow* window;



void DoNextFrame()
{
	KEY_BOARD_EVENT_MANAGER::get().ProcessInput();
	Game::get().update();

	Renderer::get().draw();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	gui::NewFrame();

	gui::Begin("Hi!! Is it alright??>>");
	gui::Text("Hello !!");
	gui::End();

	gui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(gui::GetDrawData());

	glfwPollEvents();
	auto fps = 1000 / (GAME_SYSTEM::get().tick_time().count() + 1);
	string title = "("s + to_string(fps) + " fps)"s;
	glfwSetWindowTitle(window, title.c_str());

	glfwSwapBuffers(window);
}

void MouseWheel(GLFWwindow* window, double xoffset, double yoffset)
{
	auto camera = Renderer::get().get_main_camera();
	if (yoffset > 0)
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
	DoNextFrame();
}



void CursorPos(GLFWwindow* window, double xpos, double ypos)
{

}

void MouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
	//popup_menu();

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
	};
}

void bind_key_func()
{
	KEY_BOARD_EVENT_MANAGER::get().BindKeyFunc(GLFW_KEY_LEFT_SHIFT, 
		[] { Game::get().renderer.get_main_camera()->camera_shake(0.2f); });

}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	window = glfwCreateWindow(screen.width, screen.height, "SUNKUE", NULL, NULL);
	if (window == NULL)
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

	glfwSetScrollCallback(window, MouseWheel);
	glfwSetMouseButtonCallback(window, MouseButton);
	glfwSetCursorPosCallback(window, CursorPos);
	glfwSetKeyCallback(window,
		[](GLFWwindow* window, int key, int code, int action, int modifiers)
		{ KEY_BOARD_EVENT_MANAGER::get().KeyBoard(window, key, code, action, modifiers); });

	IMGUI_CHECKVERSION();
	gui::CreateContext();
	ImGuiIO& io = gui::GetIO(); (void)io;
	gui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

	Renderer::get().reshape(screen.width, screen.height);

	bind_key_func();

	while (!glfwWindowShouldClose(window))
	{
		DoNextFrame();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	gui::DestroyContext();

	glfwTerminate();
}


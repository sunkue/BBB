
#include "stdafx.h"
#include "Renderer.h"
#include "VAO_OBJ.h"
#include "Camera.h"
#include "Game.h"

GLFWwindow* window;


void processInput(GLFWwindow* window);

void DoNextFrame(void)
{
	processInput(window);
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

void MouseWheel(GLFWwindow* window, double xoffset, double yoffset) {
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

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	if (glfwGetKey(window, 's') == GLFW_PRESS);
	if (glfwGetKey(window, 'd') == GLFW_PRESS);
	if (glfwGetKey(window, 'a') == GLFW_PRESS);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
}


void SpecialKeyInput(int key, int x, int y)
{
	//	switch (key)
	//	{
	//	case GLUT_KEY_SHIFT_L:
	//		Game::get().renderer.get_main_camera()->camera_shake(0.2f);
	//		break;
	//	default: break;
	//	}
}

void Resize(GLFWwindow* window, int w, int h)
{
	Renderer::get().reshape(w, h);
	DoNextFrame();
}

void Position(int x, int y)
{
	DoNextFrame();
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

	glfwSetFramebufferSizeCallback(window, Resize);
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) { Renderer::get().reshape(w, h); });
	glfwSetScrollCallback(window, MouseWheel);
	//glfwSetMouseButtonCallback(window, );
	//glfwSetCursorPosCallback();


	IMGUI_CHECKVERSION();
	gui::CreateContext();
	ImGuiIO& io = gui::GetIO(); (void)io;
	gui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

	while (!glfwWindowShouldClose(window))
	{
		DoNextFrame();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	gui::DestroyContext();

	glfwTerminate();
}


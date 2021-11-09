
#include "stdafx.h"
#include "Renderer.h"
#include "VAO_OBJ.h"
#include "Camera.h"
#include "Game.h"


void RenderScene(void)
{
	Game::get().renderer.draw();

	//glutSwapBuffers();
}

void Idle(void)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	RenderScene();
}

void MouseWheel(int button, int dir, int x, int y) {
	auto camera = Renderer::get().get_main_camera();
	if (dir > 0)
	{
		auto diff = camera->get_diff();
		camera->set_diff(diff * 0.875f);
		cout << diff.x << " " << diff.y << " " << diff.z << endl;
		//- 1.31538 0.789227 0
		//- 1.4798 0.88788 0
	}
	else
	{
		auto diff = camera->get_diff();
		camera->set_diff(diff * 1.125f);
		cout << diff.x << " " << diff.y << " " << diff.z << endl;
	}

	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
	//Game::get().player->apply_input_press(key);
	return;
	switch (key)
	{
	case 'w':
	case 'W':
	case 's':
	case 'S':
	case 'a':
	case 'A':
	case 'd':
	case 'D':
	case ' ':
		break;
	case 164: { /* 한글입력 */

	}break;
	default: break;
	}

	//RenderScene();
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		cout << "!";
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
	{
		cout << "?";
	}
}

void KeyInputUp(unsigned char key, int x, int y)
{
	//Game::get().player->apply_input_unpress(key);
	return;
	switch (key)
	{
	case 'w':
	case 'W':
	case 's':
	case 'S':
	case 'd':
	case 'D':
	case 'a':
	case 'A':
	case ' ':
		break;
	case 164: { /* 한글입력 */

	}break;
	default: break;
	}

	//RenderScene();
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
}

void Position(int x, int y)
{
	RenderScene();
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(screen.width, screen.height, "SUNKUE", NULL, NULL);
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
		processInput(window);
		

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		gui::NewFrame();



		gui::Begin("Hi!! Is it alright??>>");
		gui::Text("Hello !!");
		gui::End();

		gui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(gui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	gui::DestroyContext();

	glfwTerminate();

	//glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	//glutInitWindowPosition(200, 200);
	//glutInitWindowSize(g_WindowSizeX, g_WindowSizeY);
	//glutCreateWindow("OPGL_SUNKUE");
	//glewInit();
	//if (glewIsSupported("GL_VERSION_4_5"))
	//{
	//	std::cout << " GL Version is 4.5\n ";
	//}
	//else
	//{
	//	std::cout << "GLEW 4.5 not supported\n ";
	//}


	


	//glutDisplayFunc(RenderScene);
	//glutIdleFunc(Idle);
	//glutKeyboardFunc(KeyInput);
	//glutKeyboardUpFunc(KeyInputUp);
	//glutMouseFunc(MouseInput);
	//glutMouseWheelFunc(MouseWheel);
	//glutSpecialFunc(SpecialKeyInput);
	//glutReshapeFunc(Resize);
	//glutPositionFunc(Position);
	//glutCloseFunc(Close);

	//glutMainLoop();
}


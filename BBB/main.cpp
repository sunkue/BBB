

#include "stdafx.h"
#include "Renderer.h"
#include "VAO_OBJ.h"
#include "Camera.h"
#include "Game.h"
#include "SUNKUE.hpp"

int g_WindowSizeX = 500;
int g_WindowSizeY = 500;

void RenderScene(void)
{
	Game::instance().renderer.draw();

	glutSwapBuffers();
}

void Idle(void)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{ 
	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
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
		Game::instance().player->apply_input_press(key);
		break;
	case 164: { /* 한글입력 */

	}break;
	default: break;
	}
	
	RenderScene();
}

void KeyInputUp(unsigned char key, int x, int y)
{
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
		Game::instance().player->apply_input_unpress(key);
		break;
	case 164: { /* 한글입력 */

	}break;
	default: break;
	}

	RenderScene();
}

void SpecialKeyInput(int key, int x, int y)
{
	RenderScene();
}



int main(int argc, char **argv)
{

	
	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(g_WindowSizeX, g_WindowSizeY);
	glutCreateWindow("OPGL_SUNKUE");
	
	glewInit();
	if (glewIsSupported("GL_VERSION_4_5"))
	{
		std::cout << " GL Version is 4.5\n ";
	}
	else
	{
		std::cout << "GLEW 4.5 not supported\n ";
	}

	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutKeyboardUpFunc(KeyInputUp);
	glutMouseFunc(MouseInput);
	glutSpecialFunc(SpecialKeyInput);

	glutMainLoop();
}


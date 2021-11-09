#pragma once

#pragma comment(lib,"./Dependencies/OPGL/x64GL/glew32.lib")
#pragma comment(lib,"./Dependencies/OPGL/x64GL/glew32s.lib")
#pragma comment(lib,"./Dependencies/OPGL/x64GL/freeglut.lib")
#pragma comment(lib,"./Dependencies/GLFW/glfw3.lib")
#pragma comment(lib,"./Dependencies/GLFW/glfw3_mt.lib")
#pragma comment(lib,"./Dependencies/GLFW/glfw3dll.lib")

#define WIN32_LEAN_AND_MEAN


#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <cassert>
#include <type_traits>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <chrono>
#include <array>
#include <queue>
#include <memory>
#include <list>

using namespace std;
using namespace chrono;
using namespace std::literals::string_view_literals;
using clk = std::chrono::high_resolution_clock;

#pragma warning(push)
#pragma warning(disable: 26495)
#pragma warning(disable: 26812)


#include "Dependencies/glad/include/glad/glad.h"
//#include "Dependencies/glad/include/KHR/khrplatform.h"

//#include "Dependencies/OPGL/glew.h"
//#include "Dependencies/OPGL/wglew.h"
//#/include "Dependencies/OPGL/freeglut.h"
#include "Dependencies/GLFW/GLFW/glfw3.h"
#include "Dependencies/GLFW/GLFW/glfw3native.h"

#include "Dependencies/OPGL/glm/glm.hpp"
#include "Dependencies/OPGL/glm/gtx/projection.hpp"
#include "Dependencies/OPGL/glm/gtx/transform.hpp"
#include "Dependencies/OPGL/glm/gtx/quaternion.hpp"
#include "Dependencies/OPGL/glm/gtx/matrix_decompose.hpp"
#include "Dependencies/OPGL/glm/ext.hpp"


#include "Dependencies/imgui/imgui.h"
#include "Dependencies/imgui/imconfig.h"
#include "Dependencies/imgui/imgui_impl_glfw.h"
#include "Dependencies/imgui/imgui_impl_opengl3.h"
#define gui ImGui



#include "../../Share/includes.h"
#include "gl_defines.h"
#include "sunkue_glm.hpp"
#include "game_system.h"
#include "Texture.h"
#include "Light.h"
#include "Shader.h"
#include "Model.h"
#include "VAO_OBJ.h"


#pragma warning(pop)
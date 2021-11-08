#include "stdafx.h"
#include "VAO_OBJ.h"
#include "Renderer.h"


void OBJ::update_uniform_vars(const ShaderPtr& shader)const
{
	glm::mat4 m = model_mat();
	shader->set("u_m_mat", m);
}

// [-1, 1]
glm::vec3 OBJ::get_project_pos(glm::vec3 origin)
{
	auto m = model_mat();
	auto v = Renderer::get().get_main_camera()->view_mat();
	auto p = screen.proj_mat();
	auto res = p * v * m * glm::vec4{ origin ,1.f };
	return glm::vec3{ res } / res.w;
}


/// ////////////////////////////////////////////


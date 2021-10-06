#include "stdafx.h"
#include "VAO_OBJ.h"
#include "Renderer.h"


void OBJ::update_uniform_vars()const
{
	auto& renderer = Renderer::instance();
	auto camera = renderer.get_main_camera();
	glm::mat4 mvp = renderer.proj_mat() * camera->view_mat() * model_mat();
	glUniformMatrix4fv(renderer.get_uloc_mvp_mat(), 1, GL_FALSE, glm::value_ptr(mvp));

}
#include "stdafx.h"
#include "VAO_OBJ.h"
#include "Renderer.h"


void OBJ::update_uniform_vars()const
{
	auto& renderer = Renderer::instance();
	auto camera = renderer.get_main_camera();
	glm::mat4 vp = renderer.vp_mat();
	glm::mat4 m = model_mat();
	glUniformMatrix4fv(renderer.get_uloc_vp_mat(), 1, GL_FALSE, glm::value_ptr(vp));
	glUniformMatrix4fv(renderer.get_uloc_m_mat(), 1, GL_FALSE, glm::value_ptr(m));

}

/// ////////////////////////////////////////////

void Billboard::update_uniform_vars() const
{
	auto& renderer = Renderer::instance();
	auto camera = renderer.get_main_camera();
	glm::mat4 vp = renderer.vp_mat();
	glm::mat4 p = glm::translate(get_position());
	glm::mat4 rs = glm::toMat4(get_rotation()) * glm::scale(get_scale());

	glUniformMatrix4fv(renderer.get_uloc_vp_mat(), 1, GL_FALSE, glm::value_ptr(vp));
	glUniformMatrix4fv(renderer._uloc_p_mat_bs, 1, GL_FALSE, glm::value_ptr(p));
	glUniformMatrix4fv(renderer._uloc_rs_mat_bs, 1, GL_FALSE, glm::value_ptr(rs));

}

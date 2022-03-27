#include "stdafx.h"
#include "Obj.h"
#include "Renderer.h"


void Obj::load_file_impl(ifstream& file)
{
	LOAD_FILE(file, translate_);
	LOAD_FILE(file, quaternion_);
	LOAD_FILE(file, scale_);

	trans_debug();
}

void Obj::save_file_impl(ofstream& file)
{
	SAVE_FILE(file, translate_);
	SAVE_FILE(file, quaternion_);
	SAVE_FILE(file, scale_);
}

void Obj::update_uniform_vars(const ShaderPtr& shader)const
{
	shader->use();
	glm::mat4 m = model_mat();
	shader->set("u_m_mat", m);
	glUseProgram(0);
}

void Obj::draw(const ShaderPtr& shader) const
{
	model_->draw(shader);
	boundings_.draw();
}

void Obj::draw(const ShaderPtr& shader, const ModelPtr& model) const
{
	model->draw(shader);
	boundings_.draw();
}

// [-1, 1]
glm::vec3 Obj::get_project_pos(glm::vec3 origin)
{
	auto m = model_mat();
	auto v = Renderer::get().get_main_camera()->view_mat();
	auto p = screen.proj_mat();
	auto res = p * v * m * glm::vec4{ origin ,1.f };
	return glm::vec3{ res } / res.w;
}


/// ////////////////////////////////////////////

void InstancingObj::update_uniform_vars(const ShaderPtr& shader) const
{
	shader->use();
	auto gametime = static_cast<float>(GAME_SYSTEM::get().game_time().count()) / 1000.f;
	shader->set("u_time", gametime);
	shader->set("u_tex_sampler", get_textures());
	glUseProgram(0);
}

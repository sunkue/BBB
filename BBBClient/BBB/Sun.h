#pragma once

#include "FileHelper.h"
#include "Light.h"
#include "Model.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"
#include "Collision.h"

class Sky : public IDataOnFile
{
	SINGLE_TON(Sky)
	{
		sun_light_ = DirectionalLight::create();
		load("sky");
	}
	
private:
	virtual void save_file_impl(ofstream& file) final;
	virtual void load_file_impl(ifstream& file) final;

private:
	DirectionalLightPtr sun_light_;
	//ShaderPtr sun_shader_;
	glm::vec3 skycolor_top_ = glm::vec3(0.5, 0.7, 0.8) * 1.05f;
	glm::vec3 skycolor_bottom_ = glm::vec3(0.9, 0.9, 0.95);
	glm::vec3 sun_color_{ 1.0,.6,0.1 };

public:
	GET_REF(sun_light);

	void draw_gui();

	void update_uniform_vars(const ShaderPtr& sun_shader) const
	{
		sun_shader->use();
		sun_shader->set("u_sun_dir", sun_light_->direction);
		sun_shader->set("u_sun_color", sun_color_);
		sun_shader->set("u_skycolor_bottom", skycolor_bottom_);
		sun_shader->set("u_skycolor_top", skycolor_top_);
	}
};


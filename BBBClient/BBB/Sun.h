#pragma once

#include "Light.h"
#include "Model.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"
#include "Collision.h"

class Sun
{
	SINGLE_TON(Sun)
	{
		sun_light_ = DirectionalLight::create();
	}

private:
	DirectionalLightPtr sun_light_;
	//ShaderPtr sun_shader_;
	glm::vec3 skycolor_top = glm::vec3(0.5, 0.7, 0.8) * 1.05f;
	glm::vec3 skycolor_bottom = glm::vec3(0.9, 0.9, 0.95);
	glm::vec3 color{ 1.0,.6,0.1 };

public:
	void draw_gui()
	{
		gui::Begin("Sun");

		gui::End();
	}

	void update_uniform_vars(const ShaderPtr& sun_shader) const
	{
		sun_shader->use();
		sun_shader->set("u_sun_dir", sun_light_->direction);
		sun_shader->set("u_sun_color", color);
		sun_shader->set("u_skycolor_bottom", skycolor_bottom);
		sun_shader->set("u_skycolor_top", skycolor_top);
	}
};


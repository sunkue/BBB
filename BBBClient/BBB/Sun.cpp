#include "stdafx.h"
#include "Sun.h"
#include "Renderer.h"



////////////////////

void Sky::load_file_impl(ifstream& file)
{
	LOAD_FILE(file, sun_light_->direction);
	LOAD_FILE(file, skycolor_top_);
	LOAD_FILE(file, skycolor_bottom_);
	LOAD_FILE(file, sun_color_);
}

void Sky::save_file_impl(ofstream& file)
{
	SAVE_FILE(file, sun_light_->direction);
	SAVE_FILE(file, skycolor_top_);
	SAVE_FILE(file, skycolor_bottom_);
	SAVE_FILE(file, sun_color_);
}

//////////////////////////////

void Sky::draw_gui()
{
	//gui::Begin("");
	gui::Text("sky");
	
	GUISAVE();
	GUILOAD();

	auto dir_origin = sun_light_->direction;
	auto new_dir = sun_light_->direction;
	auto direction_change = gui::DragFloat3("direction", glm::value_ptr(new_dir), 0.1f);
	if (direction_change)
	{
		glm::quat rotate{ dir_origin - new_dir };
		sun_light_->direction = glm::rotate(rotate, sun_light_->direction);
	}
	gui::DragFloat3("skycolor_top", glm::value_ptr(skycolor_top_));
	gui::DragFloat3("skycolor_bottom", glm::value_ptr(skycolor_bottom_));
	gui::DragFloat3("color", glm::value_ptr(sun_color_));
	//gui::End();
}
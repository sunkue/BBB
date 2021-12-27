#include "stdafx.h"
#include "DynamicObj.h"
#include "KeyboardEvent.h"
#include "Camera.h"
#include "Game.h"


bool VehicleObj::process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key)
{
	bool pressed = (key.action != GLFW_RELEASE);
	switch (key.key)
	{
	case GLFW_KEY_W:
	{
		front_on_ = pressed;
	}
	CASE GLFW_KEY_S :
	{
		back_on_ = pressed;
	}
	CASE GLFW_KEY_A :
	{
		left_on_ = pressed;
	}
	CASE GLFW_KEY_D :
	{
		right_on_ = pressed;
	}
	CASE GLFW_KEY_SPACE :
	{
		brake_on_ = pressed;
	}
	break; default: return false;
	}

	return true;
}

void VehicleObj::update_speed(float time_elapsed)
{
	auto head_dir = get_head_dir();

	/* fric */
	auto prev_dir = glm::normalize(linear_speed_);
	auto prev_speed = glm::length(linear_speed_);
	if (prev_speed < ellipsis)
	{
		prev_dir = V_ZERO;
	}
	auto fric = -1 * prev_dir * friction_ * time_elapsed;

	/* accel */
	auto accel = head_dir * acceleration_ * time_elapsed;

	///* accumulate */
	auto new_linear_speed = linear_speed_ + accel + fric;

	/* no backward movement */
	auto new_dir = glm::normalize(new_linear_speed);
	auto new_speed = glm::length(new_linear_speed);
	if (new_speed < ellipsis)
	{
		new_dir = V_ZERO;
	}

	/* overspeed */
	bool over_speed = max_speed_ < new_speed;
	// [B] if (over_speed) { new_linear_speed = new_dir * _max_speed; }
	new_linear_speed -= new_linear_speed * over_speed;
	new_linear_speed += new_dir * max_speed_ * over_speed;

	linear_speed_ = new_linear_speed;
}

void VehicleObj::draw_gui()
{
	gui::Begin("Vehicle");
	gui::Text("This is Vehicle in real game.");
	gui::DragInt("ID", (int*)&id_);
	gui::Text("speed");
	gui::DragFloat3("linear_speed", const_cast<float*>(glm::value_ptr(linear_speed_)));
	gui::DragFloat3("angular_speed", const_cast<float*>(glm::value_ptr(angular_speed_)));
	gui::SliderFloat("max_speed", &max_speed_, 10.0f, 100.0f);
	gui::SliderFloat("acceleration", &acceleration_, 0.0f, 100.0f);
	gui::SliderFloat("friction", &friction_, 0.0f, 100.0f);
	gui::Text("state");
	gui::Checkbox("front_on", &front_on_);
	gui::Checkbox("back_on", &back_on_);
	gui::Checkbox("right_on", &right_on_);
	gui::Checkbox("left_on", &left_on_);
	gui::Checkbox("brake_on", &brake_on_);
	gui::Checkbox("use_item", &use_item_);
	gui::End();

	Obj::draw_gui();
}

void VehicleObj::update_state()
{
	const glm::vec3 _angular_power = Y_DEFAULT * 1.5f;
	constexpr float _acceleration_power = 16.f;
	constexpr float _friction_power = 2.0f;

	angular_speed_ = _angular_power * static_cast<int>(angular_control_);
	acceleration_ = _acceleration_power * static_cast<int>(accel_control_);

	// [B] if(_brake_on)_acceleration = 0;
	//_acceleration -= _acceleration * _brake_on;

	friction_ = _friction_power;
	// [B] if(_brake_on)_friction += _friction_power;
	friction_ += _friction_power * brake_on_;
	friction_ += _friction_power * brake_on_;

	int accel_control{ static_cast<int>(CONTROLL::none) };
	int angular_control{ static_cast<int>(CONTROLL::none) };
	accel_control += front_on_;
	accel_control -= back_on_;
	angular_control -= right_on_;
	angular_control += left_on_;
	accel_control_ = static_cast<CONTROLL>(accel_control);
	angular_control_ = static_cast<CONTROLL>(angular_control);
}

void VehicleObj::update_camera(Camera* camera, float time_elpased) const
{
	glm::vec3 target_dir; // using for target & position

	/* target */
	{
		const auto head_dir = get_head_dir();
		const auto moving_speed = get_linear_speed();
		const auto moving_len = glm::length(moving_speed);
		auto moving_dir = glm::normalize(moving_speed);
		if (moving_len < 0.125f)
		{
			moving_dir = head_dir;
		}
		target_dir = glm::lerp(head_dir, moving_dir, clamp(0.2f * moving_len, 0.00f, 0.4f));
		camera->set_target(get_position() + target_dir * 8.0f + Y_DEFAULT * 2.f);
	}

	/* position */
	{
		auto diff = camera->get_diff();
		auto rotate = quat_from2vectors({ diff.x, 0,diff.z }, { -target_dir.x, 0, -target_dir.z });
		const auto trans_diff = glm::translate(diff);
		const auto trans = glm::translate(get_position());
		const auto scale = glm::scale(get_scale());
		auto m = trans * glm::toMat4(rotate) * scale * trans_diff;
		camera->set_position(m * V4_DEFAULT);
	}
}


/////////////////////////////////////////////////////////

void GhostObj::update(float time_elapsed)
{
	float speed = speed_ * time_elapsed;
	if (up_on_) { move(get_up_dir() * speed); };
	if (down_on_) { move(-get_up_dir() * speed); };
	if (front_on_) { move(get_head_dir() * speed); };
	if (back_on_) { move(-get_head_dir() * speed); };
	if (right_on_) { move(get_right_dir() * speed); };
	if (left_on_) { move(-get_right_dir() * speed); };
}

void GhostObj::update_camera(Camera* camera, float time_elpased) const
{
	auto position = get_position();
	camera->set_position(position);
	camera->set_target(position + get_head_dir());
}
void GhostObj::draw_gui()
{
	gui::Begin("Ghost");
	gui::Text("This is Ghost for free moving camera");
	gui::SliderFloat("speed", &speed_, 10.0f, 500.0f);
	gui::End();
}

void GhostObj::draw(const ShaderPtr& shader) const
{

}

bool GhostObj::process_input(const MOUSE_EVENT_MANAGER::scroll_event& scroll)
{
	// 오브젝트 선택 후 스크롤로 사이즈 조정, (클릭하고 있을 때,)
	auto& mm = MOUSE_EVENT_MANAGER::get();

	if (mm.get_L_click())
	{

		return true;
	}

	if (mm.get_R_click())
	{

		return true;
	}

	if (mm.get_Wheel_click())
	{

		return true;
	}

	{
		auto speed = clamp(scroll.yoffset * speed_, -100., 100.);
		move(get_head_dir() * speed);
		return true;
	}
	return true;
}

bool GhostObj::process_input(const MOUSE_EVENT_MANAGER::button_event& button)
{
	auto& mm = MOUSE_EVENT_MANAGER::get();
	if (GLFW_MOUSE_BUTTON_MIDDLE == button.button)
	{
		if (GLFW_PRESS == button.action)
		{
			rotate(glm::inverse(get_rotation()));
		}
	}

	if (GLFW_MOUSE_BUTTON_LEFT == button.button)
	{
		if (GLFW_PRESS == button.action)
		{
			auto& renderer = Renderer::get();
			auto mouse_ray = Ray::create(mm.get_prev_x(), mm.get_prev_y());
			for (const auto& car : renderer.get_cars())
			{
				float dist;
				if (car->get_boundings().intersects(mouse_ray, dist))
				{
					renderer.set_ghost(car);
					renderer.swap_player_ghost();
					break;
				}
			}
		}
	}

	// 오브젝트 선택. 클릭시 선택 (ray_collision)
	return false;
}

bool GhostObj::process_input(const MOUSE_EVENT_MANAGER::pos_event& pos)
{
	auto& mm = MOUSE_EVENT_MANAGER::get();
	// 오브젝트 클릭중일때 선택된 오브젝트의 위치 변경, 
	if (mm.get_R_click())
	{
		if (selected_obj_)
		{
			return true;
		}
		else
		{
			// 회전
			float xoffset = pos.xpos - mm.get_prev_x(); xoffset /= 10;
			float yoffset = pos.ypos - mm.get_prev_y(); yoffset /= 10;
			auto& camera = Renderer::get().get_main_camera();
			auto xm = glm::rotate(glm::radians(-xoffset), camera->get_up());
			//auto ym = glm::rotate(glm::radians(-yoffset), camera->get_right());
			auto ym = glm::rotate(glm::radians(-yoffset), camera->get_right());

			rotate({ xm * ym });
			return true;
		}

	}
	return false;
}

bool GhostObj::process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key)
{
	bool pressed = (key.action != GLFW_RELEASE);
	switch (key.key)
	{
	case GLFW_KEY_W:
	{
		front_on_ = pressed;
	}
	CASE GLFW_KEY_S :
	{
		back_on_ = pressed;
	}
	CASE GLFW_KEY_A :
	{
		left_on_ = pressed;
	}
	CASE GLFW_KEY_D :
	{
		right_on_ = pressed;
	}
	CASE GLFW_KEY_E :
	{
		up_on_ = pressed;
	}
	CASE GLFW_KEY_Q :
	{
		down_on_ = pressed;
	}
	CASE GLFW_KEY_LEFT_SHIFT :
	{
		speed_ += pressed;
	}
	break; default: return false;
	}
	return true;
}

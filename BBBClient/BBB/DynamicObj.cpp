#include "stdafx.h"
#include "DynamicObj.h"
#include "KeyboardEvent.h"
#include "Camera.h"
#include "Game.h"


void VehicleObj::load_file_impl(ifstream& file)
{
	Obj::load_file_impl(file);
	LOAD_FILE(file, angular_power_);
	LOAD_FILE(file, acceleration_power_);
	LOAD_FILE(file, friction_power_);
	LOAD_FILE(file, max_speed_);
}

void VehicleObj::save_file_impl(ofstream& file)
{
	Obj::save_file_impl(file);
	SAVE_FILE(file, angular_power_);
	SAVE_FILE(file, acceleration_power_);
	SAVE_FILE(file, friction_power_);
	SAVE_FILE(file, max_speed_);
}

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
	CASE GLFW_KEY_LEFT_SHIFT :
	{
		draft_on_ = pressed;
		// brake_on_ = pressed;
	}
	CASE GLFW_KEY_APOSTROPHE : // '
	{
		if (pressed)
		{
			Track::get().include_obj(*reinterpret_cast<Obj*>(this), true);
		}
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
	if (true == draft_on_)
	{
		head_dir = glm::normalize(get_linear_speed());
		draft_time_ = std::min(draft_time_ + time_elapsed, 15.f);
	}
	else
	{
		head_dir *= draft_time_;
		draft_time_ = std::max(draft_time_ - time_elapsed, 1.f);
	}

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
	gui::DragInt("rank", &rank_);
	gui::DragInt("cp", (int*)&check_point_);
	gui::DragInt("lab", &lab_);
	gui::DragInt("node", &included_node_);
	gui::DragFloat3("linear_speed", glm::value_ptr(linear_speed_));
	auto speed = glm::length(linear_speed_);
	gui::DragFloat("setting", &speed);
	GUISAVE(); GUILOAD();
	gui::SliderFloat("max_speed", &max_speed_, 1.0f, 100.0f);
	gui::DragFloat("angular_power", &angular_power_, 0.125, 0.5, 5);
	gui::DragFloat("acceleration_power", &acceleration_power_, 1, 10, 50);
	gui::DragFloat("friction_power", &friction_power_, 0.125, 0.125, 15);
	gui::Text("state");
	gui::SliderFloat("acceleration", &acceleration_, -max_speed_, max_speed_);
	gui::SliderFloat("friction", &friction_, 0, max_speed_);
	gui::Checkbox("front_on", &front_on_);
	gui::Checkbox("back_on", &back_on_);
	gui::Checkbox("right_on", &right_on_);
	gui::Checkbox("left_on", &left_on_);
	gui::Checkbox("brake_on", &brake_on_);
	gui::Checkbox("draft_on", &draft_on_);
	gui::Checkbox("use_item", &use_item_);
	gui::End();

	Obj::draw_gui();
}

void VehicleObj::update_state()
{
	const glm::vec3 _angular_power = Y_DEFAULT * angular_power_;
	const float _acceleration_power = acceleration_power_;
	const float _friction_power = friction_power_;

	angular_speed_ = _angular_power * static_cast<int>(angular_control_);
	acceleration_ = _acceleration_power * static_cast<int>(accel_control_);

	// [B] if(_brake_on)_acceleration = 0;
	//_acceleration -= _acceleration * _brake_on;

	friction_ = _friction_power;
	// [B] if(_brake_on)_friction += _friction_power;
	friction_ += _friction_power * draft_on_;
	friction_ += _friction_power * draft_on_;
	friction_ += _friction_power * draft_on_;

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

void VehicleObj::regenerate()
{
	const float speed{ 0 };
	auto& node = Track::get().get_tracks()[included_node_];
	linear_speed_ = speed * node->get_front();
	auto q = sunkueglm::quat_from2vectors(get_head_dir(), node->get_front());
	rotate(q);
}

bool VehicleObj::rank_worse_than(VehicleObj& other)
{
	//lab
	if (lab_ < other.lab_)
	{
		return true;
	}

	// same lab
	// node
	assert(lab_ == other.lab_);
	auto& nodes = Track::get().get_tracks();
	auto& this_node = nodes.at(included_node_);
	auto& other_node = nodes.at(other.included_node_);
	if (this_node->get_from_start() < other_node->get_from_start())
	{
		return true;
	}

	// same lab
	// same node
	// pos
	assert(this_node == other_node);
	auto next_pos = this_node->get_next_center();
	auto this_diff = next_pos - get_position();
	auto other_diff = next_pos - other.get_position();
	return glm::length(other_diff) < glm::length(this_diff);
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void GhostObj::update(float time_elapsed)
{
	auto& camera = Game::get().renderer.get_main_camera();
	auto right = camera->get_right();
	auto up = camera->get_up();
	auto front = camera->get_look_dir();

	float speed = speed_ * time_elapsed;
	if (up_on_) { move(up * speed); };
	if (down_on_) { move(-up * speed); };
	if (front_on_) { move(front * speed); };
	if (back_on_) { move(-front * speed); };
	if (right_on_) { move(right * speed); };
	if (left_on_) { move(-right * speed); };

	if (selected_obj_)
	{
		auto set_arrow_to_target = [this](auto& x, const glm::vec3 v)
		{
			auto& t = selected_obj_;
			auto pos = t->get_position() - x->get_position();
			auto scale = glm::vec3(glm::compMax(t->get_scale())) * 0.625;
			x->move(pos);
			x->scaling(glm::vec3{ 1 } / x->get_scale());
			auto value = scale * v;
			x->scaling(value + glm::vec3{ 0.2 });
			x->move(value);
		};
		set_arrow_to_target(xAxis, X_DEFAULT);
		set_arrow_to_target(yAxis, Y_DEFAULT);
		set_arrow_to_target(zAxis, Z_DEFAULT);
	}
}

void GhostObj::update_camera(Camera* camera, float time_elpased) const
{
	auto ryaw = glm::radians(rotator_.yaw);
	auto rpitch = glm::radians(rotator_.pitch);

	glm::vec3 front;
	front.x = cos(ryaw) * cos(rpitch);
	front.y = sin(rpitch);
	front.z = sin(ryaw) * cos(rpitch);
	front = glm::normalize(front);

	auto position = get_position();
	camera->set_position(position);
	camera->set_target(position + front);
}
void GhostObj::draw_gui()
{
	gui::Begin("Ghost");
	gui::Text("This is Ghost for free moving camera");
	if (gui::Button(+cntl_mode._to_string()))
	{
		if (cntl_mode._size() == cntl_mode._value)
		{
			cntl_mode = CONTROLL_MODE::TRANSLATE;
		}
		else
		{
			cntl_mode._value++;
		}
	}
	gui::SliderFloat("speed", &speed_, 10.0f, 500.0f);
	gui::End();

	if (selected_obj_)
	{
		selected_obj_->draw_gui();
	}
}

void GhostObj::update_uniform_vars(const ShaderPtr& shader) const
{
	// none
}

void GhostObj::draw(const ShaderPtr& shader) const
{
	// draw selected obj arrows.
	switch (select_mode)
	{
	case GhostObj::SELECTED_MODE::NONE:
	{
		xAxis->update_uniform_vars(shader);
		xAxis->draw(shader);
		yAxis->update_uniform_vars(shader);
		yAxis->draw(shader);
		zAxis->update_uniform_vars(shader);
		zAxis->draw(shader);
	}
	CASE GhostObj::SELECTED_MODE::X :
	{
		xAxis->update_uniform_vars(shader);
		xAxis->draw(shader, Model::box_yellow());
		yAxis->update_uniform_vars(shader);
		yAxis->draw(shader);
		zAxis->update_uniform_vars(shader);
		zAxis->draw(shader);
	}
	CASE GhostObj::SELECTED_MODE::Y :
	{
		xAxis->update_uniform_vars(shader);
		xAxis->draw(shader);
		yAxis->update_uniform_vars(shader);
		yAxis->draw(shader, Model::box_yellow());
		zAxis->update_uniform_vars(shader);
		zAxis->draw(shader);
	}
	CASE GhostObj::SELECTED_MODE::Z :
	{
		xAxis->update_uniform_vars(shader);
		xAxis->draw(shader);
		yAxis->update_uniform_vars(shader);
		yAxis->draw(shader);
		zAxis->update_uniform_vars(shader);
		zAxis->draw(shader, Model::box_yellow());
	}
	CASE GhostObj::SELECTED_MODE::XYZ :
	{
		xAxis->update_uniform_vars(shader);
		xAxis->draw(shader, Model::box_yellow());
		yAxis->update_uniform_vars(shader);
		yAxis->draw(shader, Model::box_yellow());
		zAxis->update_uniform_vars(shader);
		zAxis->draw(shader, Model::box_yellow());
	}
	}
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
		auto& camera = Game::get().renderer.get_main_camera();
		auto front = camera->get_look_dir();

		auto speed = clamp(scroll.yoffset * speed_, -100., 100.);
		move(front * speed);
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
		select_mode = SELECTED_MODE::NONE;

		if (GLFW_PRESS == button.action)
		{
			auto& renderer = Renderer::get();
			auto mouse_ray = Ray::create(mm.get_prev_x(), mm.get_prev_y());
			float dist;

			if (xAxis->get_boundings().intersects(mouse_ray, dist))
			{
				select_mode = SELECTED_MODE::X;
				return true;
			}
			if (yAxis->get_boundings().intersects(mouse_ray, dist))
			{
				select_mode = SELECTED_MODE::Y;
				return true;
			}
			if (zAxis->get_boundings().intersects(mouse_ray, dist))
			{
				select_mode = SELECTED_MODE::Z;
				return true;
			}

			// 차량 선택
			for (const auto& car : renderer.get_cars())
			{
				if (car->get_boundings().intersects(mouse_ray, dist))
				{
					selected_obj_ = car;
					return true;
				}
			}

			// 트랙 선택
			for (const auto& track : renderer.get_track().get_tracks())
			{
				if (track->get_boundings().intersects(mouse_ray, dist))
				{
					selected_obj_ = track;
					return true;
				}
			}
		}
	}

	if (mm.get_L_click() && mm.get_R_click())
	{

		auto& renderer = Renderer::get();
		auto mouse_ray = Ray::create(mm.get_prev_x(), mm.get_prev_y());

		// 차량에 타기
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

	return false;
}

bool GhostObj::process_input(const MOUSE_EVENT_MANAGER::pos_event& pos)
{
	auto& mm = MOUSE_EVENT_MANAGER::get();

	if (mm.get_L_click())
	{
		// Axis 선택
		if (selected_obj_)
		{
			float xoffset = pos.xpos - mm.get_prev_x(); xoffset /= 1;
			float yoffset = pos.ypos - mm.get_prev_y(); yoffset /= 1;
			auto mouse_ray_prev = Ray::create(mm.get_prev_x(), mm.get_prev_y());
			auto mouse_ray = Ray::create(pos.xpos, pos.ypos);
			auto diff = mouse_ray.dir - mouse_ray_prev.dir;

		

			switch (cntl_mode)
			{
			case CONTROLL_MODE::TRANSLATE:
			{
				constexpr float power = 20; diff *= power;
				switch (select_mode)
				{
				case GhostObj::SELECTED_MODE::NONE:
				{}
				CASE GhostObj::SELECTED_MODE::X :
				{
					selected_obj_->move(diff.x* X_DEFAULT);
				}
				CASE GhostObj::SELECTED_MODE::Y :
				{
					selected_obj_->move(diff.y* Y_DEFAULT);
				}
				CASE GhostObj::SELECTED_MODE::Z :
				{
					selected_obj_->move(diff.z* Z_DEFAULT);
				}
				CASE GhostObj::SELECTED_MODE::XYZ :
				{
					selected_obj_->move(diff);
				}
				}
			}
			CASE CONTROLL_MODE::ROTATE :
			{
				constexpr float power = 5; diff *= power;
				float r_click = mm.get_R_click() - 0.5;
				r_click /= abs(r_click);
				
				switch (select_mode)
				{
				case GhostObj::SELECTED_MODE::NONE:
				{}
				CASE GhostObj::SELECTED_MODE::X :
				{
					selected_obj_->rotate(glm::rotate(r_click * abs(diff.x), X_DEFAULT));
					//selected_obj_->rotate(glm::rotate(r_click * abs(diff.x), selected_obj_->get_head_dir()));
				}
				CASE GhostObj::SELECTED_MODE::Y :
				{
					selected_obj_->rotate(glm::rotate(r_click * abs(diff.y), Y_DEFAULT));
					//selected_obj_->rotate(glm::rotate(r_click * abs(diff.y), selected_obj_->get_up_dir()));
				}
				CASE GhostObj::SELECTED_MODE::Z :
				{
					selected_obj_->rotate(glm::rotate(r_click * abs(diff.z), Z_DEFAULT));
					//selected_obj_->rotate(glm::rotate(r_click * abs(diff.z), selected_obj_->get_right_dir()));
				}
				CASE GhostObj::SELECTED_MODE::XYZ :
				{

				}
				}
			}
			CASE CONTROLL_MODE::SCALE :
			{
				switch (select_mode)
				{
				case GhostObj::SELECTED_MODE::NONE:
				{}
				CASE GhostObj::SELECTED_MODE::X :
				{
					selected_obj_->scaling(diff.x * X_DEFAULT + glm::vec3(1));
				}
				CASE GhostObj::SELECTED_MODE::Y :
				{
					selected_obj_->scaling(diff.y * Y_DEFAULT + glm::vec3(1));
				}
				CASE GhostObj::SELECTED_MODE::Z :
				{
					selected_obj_->scaling(diff.z * Z_DEFAULT + glm::vec3(1));
				}
				CASE GhostObj::SELECTED_MODE::XYZ :
				{

				}
				}
			}
			}


		}
		return true;
	}


	if (mm.get_R_click())
	{
		{
			// 회전
			float xoffset = pos.xpos - mm.get_prev_x(); xoffset /= 10;
			float yoffset = pos.ypos - mm.get_prev_y(); yoffset /= 10;
			yoffset = -yoffset;

			rotator_.yaw += xoffset;
			rotator_.pitch += yoffset;

			if (bool constrainPitch = true)
			{
				if (rotator_.pitch > 89.0f)
					rotator_.pitch = 89.0f;
				if (rotator_.pitch < -89.0f)
					rotator_.pitch = -89.0f;
			}

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
		speed_ += 5 * pressed;
	}
	CASE GLFW_KEY_SPACE :
	{
		move(Renderer::get().get_ghost()->get_position() - get_position());
	}
	break; default: return false;
	}
	return true;
}


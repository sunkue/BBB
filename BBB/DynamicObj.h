#pragma once

#include "VAO_OBJ.h"

/* moving obj */
class DynamicObj
{
public:
	explicit DynamicObj(ObjDataPtr obj_data, GLuint shader)
		:_obj{ make_shared<OBJ>(obj_data, shader) } {}

	ObjPtr get_obj() { return _obj; };
public:
	glm::vec3 get_speed() { return _linear_speed; }
	glm::vec3 get_moving_dir() { return glm::normalize(_linear_speed); }
protected:
	void update(float time_elapsed)
	{
		update_rotate(time_elapsed);
		update_speed(time_elapsed);
		update_movement(time_elapsed);
	}

private:
	void update_rotate(float time_elapsed)
	{
		/* rotate */
		_obj->rotate(_angular_speed * time_elapsed);
	}

	void update_speed(float time_elapsed)
	{
		auto head_dir = _obj->get_head_dir();

		/* fric */
		auto prev_dir = glm::normalize(_linear_speed);
		auto fric = -1 * prev_dir * _friction * time_elapsed;

		/* accel */
		auto accel = head_dir * _acceleration * time_elapsed;

		///* accumulate */
		auto new_linear_speed = _linear_speed + accel + fric;
		/* no backward movement */
		auto new_dir = glm::normalize(new_linear_speed);
	

		/* overspeed */
		auto new_speed = glm::length(new_linear_speed);
		bool over_speed = _max_speed < new_speed;
		// [B] if (over_speed) { new_linear_speed = new_dir * _max_speed; }
		new_linear_speed -= new_linear_speed * over_speed;
		new_linear_speed += new_dir * _max_speed * over_speed;

		_linear_speed = new_linear_speed;
	}

	void update_movement(float time_elapsed)
	{
		_obj->move(_linear_speed * time_elapsed);
	}

public:
	static constexpr float _max_speed = 15.0f;

protected:
	glm::vec3 _linear_speed{1.0f,0.0f,0.0f};
	glm::vec3 _angular_speed{};
	float _acceleration = 0.f;
	float _friction = 0.f;

protected:
	ObjPtr _obj;

};


/* controllable obj */
class ControllObj : public DynamicObj
{
public:
	enum class CONTROLL { negative = -1, none = 0, positive = 1 };
	
public:
	explicit ControllObj(ObjDataPtr obj_data, GLuint shader)
		: DynamicObj{ obj_data, shader }
	{}

public:
	void update(float time_elapsed)
	{
		update_state();
		DynamicObj::update(time_elapsed);
	}

private:
	void update_state()
	{
		const glm::vec3 _angular_power = Y_DEFAULT * 3.0f;
		constexpr float _acceleration_power = 16.f;
		constexpr float _friction_power = 2.0f;

		_angular_speed = _angular_power * static_cast<int>(_angular_control);
		_acceleration = _acceleration_power * static_cast<int>(_accel_control);

		// [B] if(_brake_on)_acceleration = 0;
		//_acceleration -= _acceleration * _brake_on;

		_friction = _friction_power;
		// [B] if(_brake_on)_friction += _friction_power;
		_friction += _friction_power * _brake_on;
		_friction += _friction_power * _brake_on;
	}

public:
	void apply_input_press(UCHAR key);
	void apply_input_unpress(UCHAR key);

private:
	CONTROLL _accel_control{ CONTROLL::none };
	CONTROLL _angular_control{ CONTROLL::none };
	bool _brake_on = false;
	bool _use_item = false;

};
using Player0Ptr = shared_ptr<ControllObj>;



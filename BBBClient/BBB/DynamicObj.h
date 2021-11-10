#pragma once

#include "VAO_OBJ.h"
#include "KeyboardEvent.h"

/* moving obj */
class DynamicObj : public Obj
{
public:
	explicit DynamicObj(const ModelPtr& model)
		:Obj{ model } {}

public:
	GET(linear_speed);
	glm::vec3 get_moving_dir()const { return glm::normalize(linear_speed_); }
	

protected:
	virtual void update(float time_elapsed) override
	{
		update_rotate(time_elapsed);
		update_speed(time_elapsed);
		update_movement(time_elapsed);

	}

	virtual void update_camera(class Camera* camera, float time_elpased) const override
	{
		
	}

private:
	void update_rotate(float time_elapsed)
	{
		/* rotate */
		rotate(angular_speed_ * time_elapsed);
	}

	void update_speed(float time_elapsed)
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

	void update_movement(float time_elapsed)
	{
		move(linear_speed_ * time_elapsed);
	}

private:
	float max_speed_ = 35.0f;

protected:
	glm::vec3 linear_speed_{ 1.0f,0.0f,0.0f };
	glm::vec3 angular_speed_{};

	float acceleration_ = 0.f;
	float friction_ = 0.f;

};

class GhostObj : public DynamicObj
{

};

/* controllable obj */
class VehicleObj : public DynamicObj
{
public:
	enum class CONTROLL { negative = -1, none = 0, positive = 1 };

public:
	explicit VehicleObj(size_t id, const ModelPtr& model)
		: DynamicObj{ model }, id_{ id }
	{}

public:
	virtual void update(float time_elapsed) override
	{
		update_state();
		DynamicObj::update(time_elapsed);
	}

private:
	void update_state();

	virtual void update_camera(class Camera* camera, float time_elpased) const override;

	
public:
	bool process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key);

private:
	CONTROLL accel_control_{ CONTROLL::none };
	CONTROLL angular_control_{ CONTROLL::none };

	bool up_on_ = false;
	bool down_on_ = false;
	bool right_on_ = false;
	bool left_on_ = false;

	bool brake_on_ = false;
	bool use_item_ = false;

	size_t id_;
};
using Player0Ptr = shared_ptr<VehicleObj>;



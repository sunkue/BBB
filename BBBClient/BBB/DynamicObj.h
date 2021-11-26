#pragma once

#include "VAO_OBJ.h"
#include "KeyboardEvent.h"

class Camera;

/* moving obj */
class DynamicObj : public Obj
{
public:
	explicit DynamicObj(const ModelPtr& model)
		:Obj{ model } {}
	virtual ~DynamicObj() {};

public:
	GET(linear_speed);
	glm::vec3 get_moving_dir()const { return glm::normalize(linear_speed_); }
	

public:
	virtual void update(float time_elapsed) override
	{
		update_speed(time_elapsed);
		update_rotate(time_elapsed);
		update_movement(time_elapsed);
	}

private:
	void update_rotate(float time_elapsed)
	{
		rotate(angular_speed_ * time_elapsed);
	}

	virtual void update_speed(float time_elapsed) {};

	void update_movement(float time_elapsed)
	{
		move(linear_speed_ * time_elapsed);
	}

protected:
	glm::vec3 linear_speed_{ 1.0f,0.0f,0.0f };
	glm::vec3 angular_speed_{};
};



using GhostPtr = unique_ptr<class GhostObj>;
class GhostObj : public DynamicObj
{
public:
	GhostObj(const ModelPtr& model) :DynamicObj{ model } {}
public:
	virtual void update(float time_elapsed) override;
	virtual void update_camera(Camera* camera, float time_elpased) const override;
	virtual void draw(const ShaderPtr& shader)const override;
	virtual void draw_gui() override;
	virtual bool process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key) override;
	virtual bool process_input(const MOUSE_EVENT_MANAGER::scroll_event& scroll) override;
	virtual bool process_input(const MOUSE_EVENT_MANAGER::button_event& button) override;
	virtual bool process_input(const MOUSE_EVENT_MANAGER::pos_event& pos) override;

private:
	ObjPtr selected_obj_;

private:
	float speed_ = 5.0f;
	bool up_on_ = false;
	bool down_on_ = false;
	bool front_on_ = false;
	bool back_on_ = false;
	bool right_on_ = false;
	bool left_on_ = false;
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
	
	virtual void update_speed(float time_elapsed) override;

	virtual void draw_gui() override;
private:
	void update_state();

	virtual void update_camera(Camera* camera, float time_elpased) const override;

public:
	virtual bool process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key) override;

private:
	float max_speed_ = 35.0f;

	float acceleration_ = 0.f;
	float friction_ = 0.f;

	CONTROLL accel_control_{ CONTROLL::none };
	CONTROLL angular_control_{ CONTROLL::none };

	bool front_on_ = false;
	bool back_on_ = false;
	bool right_on_ = false;
	bool left_on_ = false;

	bool brake_on_ = false;
	bool use_item_ = false;

	size_t id_;
};

using Player0Ptr = shared_ptr<DynamicObj>;



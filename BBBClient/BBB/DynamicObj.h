#pragma once

#include "Obj.h"
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

#pragma warning(push)
#pragma warning(disable: 26812)
#pragma warning(disable: 26495)
BETTER_ENUM
(
	CONTROLL_MODE, int
	, TRANSLATE = 1
	, ROTATE
	, SCALE
);
#pragma warning(pop)

using GhostPtr = unique_ptr<class GhostObj>;
class GhostObj : public DynamicObj
{
public:
	GhostObj(const ModelPtr& model) :DynamicObj{ model } {}
public:
	virtual void update(float time_elapsed) override;
	virtual void update_camera(Camera* camera, float time_elpased) const override;
	virtual void draw(const ShaderPtr& shader)const override;
	virtual void update_uniform_vars(const ShaderPtr& shader)const override;
	virtual void draw_gui() override;
	virtual bool process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key) override;
	virtual bool process_input(const MOUSE_EVENT_MANAGER::scroll_event& scroll) override;
	virtual bool process_input(const MOUSE_EVENT_MANAGER::button_event& button) override;
	virtual bool process_input(const MOUSE_EVENT_MANAGER::pos_event& pos) override;

private:
	ObjPtr xAxis = make_shared<Obj>(Model::box_red());
	ObjPtr yAxis = make_shared<Obj>(Model::box_blue());
	ObjPtr zAxis = make_shared<Obj>(Model::box_green());

	enum class SELECTED_MODE
	{
		NONE,
		X,
		Y,
		Z,
		XYZ
	} select_mode;

public:
	CONTROLL_MODE cntl_mode = CONTROLL_MODE::TRANSLATE;

public:
	GET(selected_obj);
private:
	ObjPtr selected_obj_;
	rotator rotator_;

private:
	float speed_ = 25.0f;
	bool up_on_ = false;
	bool down_on_ = false;
	bool front_on_ = false;
	bool back_on_ = false;
	bool right_on_ = false;
	bool left_on_ = false;
};

/* controllable obj */
using VehiclePtr = shared_ptr<class VehicleObj>;
class VehicleObj : public DynamicObj
{
private:
	virtual void save_file_impl(ofstream& file) final;
	virtual void load_file_impl(ifstream& file) final;

public:
	enum class CONTROLL { negative = -1, none = 0, positive = 1 };

public:
	explicit VehicleObj(size_t id, const ModelPtr& model)
		: DynamicObj{ model }, id_{ id }
	{
		load("car" + to_string(id));
		get_boundings().load("carbounding");
	}

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
public:
	void regenerate();
public:
	GET(rank);
	SET(rank);
private:
	float angular_power_ = 1.5f;
	float acceleration_power_ = 16.f;
	float friction_power_ = 2.0f;
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
	bool draft_on_ = false;
	float draft_time_ = 1;
	bool use_item_ = false;

	int rank_ = 0;
	int lab_ = -1;


public:
	SET(included_node);
private:
	int included_node_ = 0;

public:
	bool rank_worse_than(VehicleObj& other);

public:
	enum class CHECK_POINT
	{
		none = 0,
		begin = 1,
		check1,
		check2,
	};

	GET(check_point);
	SET(check_point);

	void clear_lab() { lab_ += 1; };
private:
	CHECK_POINT check_point_ = CHECK_POINT::check2;

private:
	size_t id_;
};


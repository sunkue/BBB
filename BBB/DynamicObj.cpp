#include "stdafx.h"
#include "DynamicObj.h"

//prefix
inline ControllObj::CONTROLL operator++(ControllObj::CONTROLL& c) {
	typedef std::underlying_type_t<ControllObj::CONTROLL> ut;
	ut temp = (ut)c + 1;
	assert(SUNKUE::optimize::abs(temp) < 2);
	c = ControllObj::CONTROLL(temp);
	return c;
}
inline ControllObj::CONTROLL operator--(ControllObj::CONTROLL& c) {
	typedef std::underlying_type_t<ControllObj::CONTROLL> ut;
	ut temp = (ut)c - 1;
	assert(SUNKUE::optimize::abs(temp) < 2);
	c = ControllObj::CONTROLL(temp);
	return c;
}

void ControllObj::apply_input_press(UCHAR key)
{
	switch (key)
	{
	case 'w':
	case 'W':
		_accel_control = CONTROLL::positive;
//		++_accel_control;
		break;
	case 's':
	case 'S':
		_accel_control = CONTROLL::negative;
	//	--_accel_control;
		break;
	case 'a':
	case 'A':
		_angular_control = CONTROLL::positive;
	//	--_angular_control;
		break;
	case 'd':
	case 'D':
		_angular_control = CONTROLL::negative;
	//	++_angular_control;
		break;
	case ' ':
		_brake_on = true;
		break;
	default:
		break;
	}
	//cout << _obj->get_position().x << " ";
	//cout << _obj->get_position().y << " ";
	//cout << _obj->get_position().z << endl;
}

void ControllObj::apply_input_unpress(UCHAR key)
{
	switch (key)
	{
	case 'w':
	case 'W':
		_accel_control = CONTROLL::none;
//		--_accel_control;
		break;
	case 's':
	case 'S':
		_accel_control = CONTROLL::none;
//		++_accel_control;
		break;
	case 'a':
	case 'A':
		_angular_control = CONTROLL::none;
	//	++_angular_control;
		break;
	case 'd':
	case 'D':
		_angular_control = CONTROLL::none;
//		--_angular_control;
		break;
	case ' ':
		_brake_on = false;
		break;
	default:
		break;
	}
}
#include "stdafx.h"
#include "DynamicObj.h"

void ControllObj::apply_input_press(UCHAR key)
{
	switch (key)
	{
	case 'w':
	case 'W':
		_up_on = true;
		break;
	case 's':
	case 'S':
		_down_on = true;
		break;
	case 'd':
	case 'D':
		_right_on = true;
		break;
	case 'a':
	case 'A':
		_left_on = true;
		break;
	case ' ':
		_brake_on = true;
		break;
	default:
		break;
	}
}

void ControllObj::apply_input_unpress(UCHAR key)
{
	switch (key)
	{
	case 'w':
	case 'W':
		_up_on = false;
		break;
	case 's':
	case 'S':
		_down_on = false;
		break;
	case 'd':
	case 'D':
		_right_on = false;
		break;
	case 'a':
	case 'A':
		_left_on = false;
		break;
	case ' ':
		_brake_on = false;
		break;
	default:
		break;
	}
}
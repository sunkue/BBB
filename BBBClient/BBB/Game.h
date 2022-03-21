#pragma once
#include "game_system.h"
#include "Renderer.h"
#include "DynamicObj.h"

class Game
{
	SINGLE_TON(Game) = default;

public:
	void update()
	{
		//
		renderer.get_track().update();
		update_ranks();
		/// 
		float milli_tick = static_cast<float>(GAME_SYSTEM::get().tick_time().count());
		float tick = milli_tick / 1000.f;
		renderer.get_player()->update(tick);
		renderer.get_main_camera()->update(tick);
		GAME_SYSTEM::get().tick();
	}

	void update_ranks();


public:
	Renderer& renderer = Renderer::get();
	GAME_SYSTEM& game_system = GAME_SYSTEM::get();

	ObjPtr player = renderer.get_player();
	GLFWwindow* window = nullptr;
};


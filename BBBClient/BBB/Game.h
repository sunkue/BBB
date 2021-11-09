#pragma once
#include "game_system.h"
#include "Renderer.h"
#include "DynamicObj.h"

class Game
{
public:
	static Game& get()
	{
		static Game _instance;
		return _instance;
	}

	void update()
	{
		float milli_tick = static_cast<float>(GAME_SYSTEM::get().tick_time().count());
		float tick = milli_tick / 1000.f;
		renderer.get_player()->update(tick);
		renderer.get_main_camera()->update(tick);
		GAME_SYSTEM::get().tick();
	}
public:
	Renderer& renderer = Renderer::get();
	GAME_SYSTEM& game_system = GAME_SYSTEM::get();

	Player0Ptr player;

public:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

private:
	Game() 
	{
		init();
	};

	void init()
	{
		player = renderer.get_player();
	}
};


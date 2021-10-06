#pragma once


/////////////////////////////////////////////////////////////

/* logic machine */
class GAME_SYSTEM
{
public:

	static GAME_SYSTEM& instance()
	{
		static GAME_SYSTEM _instance;
		return _instance;
	}

	void tick() 
	{
		auto now = clk::now();
		_elapsed_time = duration_cast<milliseconds>(now - _prev_time_point);
		_prev_time_point = now;
	};

	milliseconds tick_time()
	{
		return _elapsed_time;
	}

private:
	GAME_SYSTEM() :_prev_time_point{ clk::now() } {};
	~GAME_SYSTEM() {};

	GAME_SYSTEM(const GAME_SYSTEM&) = delete;
	GAME_SYSTEM& operator=(const GAME_SYSTEM&) = delete;

private:
	milliseconds _elapsed_time{};
	clk::time_point _prev_time_point;

	
};

/// //////////////////////////////////////////////////




///////////////////////////////////////



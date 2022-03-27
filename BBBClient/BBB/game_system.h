#pragma once


/////////////////////////////////////////////////////////////

/* logic machine */
class GAME_SYSTEM
{
	SINGLE_TON(GAME_SYSTEM) = default;

public:
	void tick() 
	{
		auto now = clk::now();
		_elapsed_time = duration_cast<milliseconds>(now - _prev_time_point);
		_game_time += _elapsed_time; // should be overflow
		_prev_time_point = now;
	};

	milliseconds tick_time()
	{
		return _elapsed_time;
	}

	float tick_timef()
	{
		return _elapsed_time.count()/1000.f;
	}

	milliseconds game_time()
	{
		return _game_time;
	}

private:
	milliseconds _elapsed_time{};
	milliseconds _game_time{};
	clk::time_point _prev_time_point{ clk::now() };
	
};

/// //////////////////////////////////////////////////




///////////////////////////////////////



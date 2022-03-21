#include "stdafx.h"
#include "Game.h"
#include "DynamicObj.h"

void Game::update_ranks()
{
	// copy overhead // shared_ptr => 전부 제거?
	auto cars = renderer.get_cars();

	std::sort(cars.begin(), cars.end(),
		[](VehiclePtr& a, VehiclePtr& b)
		{
			return !a->rank_worse_than(*b);
		});

	for (int i = 0; i < cars.size(); i++)
	{
		cars[i]->set_rank(i + 1);
	}
}

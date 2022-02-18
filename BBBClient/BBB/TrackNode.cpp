#include "stdafx.h"
#include "TrackNode.h"

void TrackNode::draw_gui()
{
	gui::Begin(("TrackNode::" + to_string(id)).c_str());

	gui::Text("Prev_nodes");
	for (const auto& prev : prev_nodes_)
	{
		gui::InputInt("prev_ID", (int*)&prev->id);
	}

	gui::Text("Next_nodes");
	for (const auto& next : next_nodes_)
	{
		gui::InputInt("next_ID", (int*)&next->id);
	}

	gui::End();

	Obj::draw_gui();
}

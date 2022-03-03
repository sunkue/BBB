#include "stdafx.h"
#include "TrackNode.h"
#include "Renderer.h"

void TrackNode::draw_gui()
{
	gui::Begin(("TrackNode::" + to_string(id)).c_str());

	gui::Text("Prev_nodes");
	for (const auto& prev : prev_nodes_)
	{
		gui::InputInt("prev_ID", (int*)&prev->id);
		// => rejoint
	}

	gui::Text("Next_nodes");
	for (const auto& next : next_nodes_)
	{
		gui::InputInt("next_ID", (int*)&next->id);
		// => rejoint
	}

	gui::End();

	Obj::draw_gui();
}


//////////////////////

static const glm::vec2 edgescale = glm::vec2(0.2f);

void TrackNode::draw_prev_edge(const ShaderPtr& shader, TrackNode* prev) const
{
	shader->use();

	auto nodepos = get_position();
	auto diff = nodepos - prev->get_position();
	auto dir = glm::normalize(diff);
	auto length = glm::length(diff);
	auto quatlen = length / 4;

	auto center = nodepos - dir * quatlen;
	auto rotate = glm::inverse(sunkueglm::quat_from2vectors(dir));
	auto scale = glm::vec3{ quatlen, edgescale };

	glm::mat4 m
		= glm::translate(center)
		* glm::toMat4(rotate)
		* glm::scale(scale);

	shader->set("u_m_mat", m);

	Model::box_yellow()->draw(shader);
}

void TrackNode::draw_next_edge(const ShaderPtr& shader, TrackNode* next) const
{
	shader->use();

	auto nodepos = get_position();
	auto diff = next->get_position() - nodepos;
	auto dir = glm::normalize(diff);
	auto length = glm::length(diff);
	auto quatlen = length / 4;

	auto center = nodepos + dir * quatlen;
	auto rotate = glm::inverse(sunkueglm::quat_from2vectors(dir));
	auto scale = glm::vec3{ quatlen, edgescale };

	glm::mat4 m
		= glm::translate(center)
		* glm::toMat4(rotate)
		* glm::scale(scale);

	shader->set("u_m_mat", m);

	Model::box_green()->draw(shader);
}

/////////////////

void TrackNode::draw_edges(const ShaderPtr& shader) const
{
	for (auto& prev : get_prev_nodes())
	{
		draw_prev_edge(shader, prev);
	}

	for (auto& next : get_next_nodes())
	{
		draw_next_edge(shader, next);
	}
}

////////////////////////////////////////////////


void Track::draw(const ShaderPtr& shader)
{
	for (auto& node : tracks_)
	{
		node->update_uniform_vars(shader);

		if (const auto ghost_player = dynamic_cast<GhostObj*>(Renderer::get().get_player().get()))
		{
			const auto& selected_obj = ghost_player->get_selected_obj();

			if (const auto slected_node = dynamic_cast<TrackNode*>(selected_obj.get()))
			{
				{
					// check is node slectedNode.
					if (slected_node == node.get())
					{
						node->draw(shader, Model::box_blue()); // 파랑박스
						continue;
					}
				}

				{
					// check is node in slectedNode's prevlist.
					const auto& fail = slected_node->get_prev_nodes().cend();
					const auto& is_prev = std::find(cALLOF(slected_node->get_prev_nodes()), node.get());

					if (fail != is_prev)
					{
						node->draw(shader, Model::box_yellow()); // 노랑박스
						continue;
					}
				}

				{
					// check is node in slectedNode's nextlist.
					const auto& fail = slected_node->get_next_nodes().cend();
					const auto& is_next = std::find(cALLOF(slected_node->get_next_nodes()), node.get());

					if (fail != is_next)
					{
						node->draw(shader, Model::box_green()); // 초록박스
						continue;
					}
				}
			}
		}

		node->draw(shader);
	}

	draw_edges(shader);
}

void Track::draw_edges(const ShaderPtr& shader) const
{
	if (draw_all_edges_)
	{
		for (auto& node : tracks_)
		{
			node->draw_edges(shader);
		}

		return;
	}

	const TrackNode* selected_node = nullptr;

	if (const auto ghost_player = dynamic_cast<GhostObj*>(Renderer::get().get_player().get()))
	{
		const auto& selected_obj = ghost_player->get_selected_obj();

		selected_node = dynamic_cast<TrackNode*>(selected_obj.get());
	}

	if (selected_node)
	{
		if (draw_nearby_edges_)
		{
			for (auto& prev : selected_node->get_prev_nodes())
			{
				prev->draw_edges(shader);
			}

			for (auto& next : selected_node->get_next_nodes())
			{
				next->draw_edges(shader);
			}
		}

		if (draw_select_edges_)
		{
			selected_node->draw_edges(shader);

			return;
		}
	}
}

void Track::draw_gui()
{
	gui::Begin("Track");

	gui::BeginChild("Show Edge", ImVec2{ 0,0 }, true);
	gui::Text("Show Edge");
	if (gui::Checkbox("all", &draw_all_edges_))
	{
		draw_select_edges_ = draw_nearby_edges_ = draw_all_edges_;
	}
	if (gui::Checkbox("nearby", &draw_nearby_edges_))
	{
		draw_select_edges_ += draw_nearby_edges_ += draw_all_edges_;
		draw_all_edges_ = false;
	}
	if (gui::Checkbox("select", &draw_select_edges_))
	{
		draw_select_edges_ += draw_all_edges_;
		draw_nearby_edges_ = false;
		draw_all_edges_ = false;
	}
	gui::EndChild();

	gui::End();

	// track 추가 버튼...
	// 경로보기.. (노드 한개, 앞뒤노드 전부, 전체.)
	// enable collide
	// enable regen
}

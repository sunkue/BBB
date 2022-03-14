#include "stdafx.h"
#include "TrackNode.h"
#include "Renderer.h"

//////////////////////

void TrackNode::save_file_impl(ofstream& file)
{
	Obj::save_file_impl(file);
	SAVE_FILE(file, id);
}

void TrackNode::load_file_impl(ifstream& file)
{
	Obj::load_file_impl(file);
	LOAD_FILE(file, id);
}

void TrackNode::update_front()
{
	const glm::vec3 pos = get_position();
	glm::vec3 front{ 0 };
	for (const auto& next : next_nodes_)
	{
		auto diff = next->get_position() - pos;
		front += diff;
	};

	front /= next_nodes_.size();

	front_ = glm::normalize(front);
}

void TrackNode::update_rotate()
{
	auto prev_q = get_rotation();
	auto u = X_DEFAULT;
	auto v = get_next_center() - get_prev_center();
	auto q = quat_from2vectors(u, v);
	
	rotate(glm::inverse(prev_q));
	rotate(q);
}

void TrackNode::update_scale()
{

}

void TrackNode::update()
{
	for (const auto obj : joined_objs_)
	{
		// detach
		if (false == check_include(*obj))
		{
			process_detach(*obj);
		}

		// dir
		if (auto vehicle = dynamic_cast<VehicleObj* const>(obj))
		{
			auto head = vehicle->get_head_dir();
			auto drive_good_dir = glm::dot(head, front_) >= 0 ? true : false;
			if (drive_good_dir)
			{
				cout << "good dir" << endl;
			}
			else
			{
				cout << "bad dir" << endl;
				// 일정시간 지나면 자동복귀..? 1초? 3초?
			}
		}
	}



}

void TrackNode::process_collide()
{
	process_collide(this);

	for (auto& prev : prev_nodes_)
	{
		process_collide(prev);
	}

	for (auto& next : next_nodes_)
	{
		process_collide(next);
	}
}

void TrackNode::process_collide(TrackNode* node)
{
	for (const auto& obj : joined_objs_)
	{
		for (const auto& other : node->joined_objs_)
		{
			if (obj == other)
			{
				continue;
			}

			obj->collision_detect(*other); // instersect로 검사 후 액션.
		}
	}
}

void TrackNode::draw_gui()
{
	gui::Begin(("TrackNode::" + to_string(id)).c_str());
	if (gui::Button("u_front"))
	{
		update_front();
	}
	if (gui::Button("u_rotate"))
	{
		update_rotate();
	}
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

void TrackNode::draw_front_edge(const ShaderPtr& shader) const
{
	shader->use();

	auto nodepos = get_position();
	auto dir = front_;
	auto length = glm::length(get_scale());

	auto center = nodepos + dir * length + glm::vec3(0, 1, 0);
	auto rotate = glm::inverse(sunkueglm::quat_from2vectors(dir));
	auto scale = glm::vec3{ length, edgescale * 0.4f };

	glm::mat4 m
		= glm::translate(center)
		* glm::toMat4(rotate)
		* glm::scale(scale);

	shader->set("u_m_mat", m);

	Model::box()->draw(shader);
}

void TrackNode::add_prev(TrackNode* prev, bool joint_them)
{
	if (nullptr == prev) return;

	prev_nodes_.emplace_back(prev);
	if (joint_them)
	{
		prev->add_next(this, false);
	}
}

void TrackNode::add_next(TrackNode* next, bool joint_them)
{
	if (nullptr == next) return;

	next_nodes_.emplace_back(next);
	if (joint_them)
	{
		next->add_prev(this, false);
	}
}

void TrackNode::draw_edges(const ShaderPtr& shader) const
{
	draw_front_edge(shader);

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
Track::Track()
{
	load("track");
}

void Track::save_file_impl(ofstream& file)
{
	//track-
	{
		int track_nums = tracks_.size();
		SAVE_FILE(file, track_nums);


		for (int id = 0; id < track_nums; id++)
		{
			tracks_.at(id)->save();
		}

		//edge
		for (const auto& node : tracks_)
		{
			auto& prevs = node->get_prev_nodes();
			vector<int> prev_ids;
			for (const auto& prev : prevs)
			{
				prev_ids.emplace_back(prev->id);
			}
			SAVE_FILE(file, prev_ids);


			auto& nexts = node->get_next_nodes();
			vector<int> next_ids;
			for (const auto& next : nexts)
			{
				next_ids.emplace_back(next->id);
			}
			SAVE_FILE(file, next_ids);
		}
	}
}

void Track::load_file_impl(ifstream& file)
{
	//track
	{
		int track_nums; LOAD_FILE(file, track_nums);
		tracks_.clear();
		tracks_.reserve(track_nums);

		auto model = Model::box(); // Model::no_model();
		for (int id = 0; id < track_nums; id++)
		{
			const string HEADER_NODE_FILE = "tracknode";
			string name_node_file = HEADER_NODE_FILE;
			name_node_file.append(std::to_string(id));

			tracks_.emplace_back(make_shared<TrackNode>(model));
			tracks_.back()->load(name_node_file);
		}

		//edge
		for (const auto& node : tracks_)
		{
			vector<int> prev_ids; LOAD_FILE(file, prev_ids);
			for (const auto ids : prev_ids)
			{
				node->add_prev(tracks_[ids].get(), false);
			}

			vector<int> next_ids; LOAD_FILE(file, next_ids);
			for (const auto ids : next_ids)
			{
				node->add_next(tracks_[ids].get(), false);
			}
		}
	}

	for (auto& node : tracks_)
	{
		node->update_front();
	}
}

void Track::draw(const ShaderPtr& shader)
{
	if (false == draw_) return;

	for (auto& node : tracks_)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL - wiremode_);

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

	GUISAVE();
	GUILOAD();

	gui::Checkbox("show", &draw_);
	gui::Checkbox("wiremode", &wiremode_);

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

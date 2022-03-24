#include "stdafx.h"
#include "TrackNode.h"
#include "Renderer.h"

//////////////////////

void TrackNode::save_file_impl(ofstream& file)
{
	Obj::save_file_impl(file);
	SAVE_FILE(file, id_);
}

void TrackNode::load_file_impl(ifstream& file)
{
	Obj::load_file_impl(file);
	LOAD_FILE(file, id_);
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
	auto pos = get_position();
	auto next_pos = get_next_center();
	auto prev_pos = get_prev_center();
	auto mid_pos = (next_pos + prev_pos) / 2;

	auto next_diff = next_pos - pos;
	auto prev_diff = pos - prev_pos;
	auto v = next_pos - prev_pos;


	float next_cos = cos_from2vectors(next_diff, v);
	float next_theta = glm::acos(next_cos);
	float next_sin = glm::sin(glm::radians(90 - glm::degrees(next_theta)));
	float next_length = glm::length(next_diff) * next_sin;

	float prev_cos = cos_from2vectors(prev_diff, v);
	float prev_theta = glm::acos(prev_cos);
	float prev_sin = glm::sin(glm::radians(90 - glm::degrees(prev_theta)));
	float prev_length = glm::length(prev_diff) * prev_sin;

	auto s = get_scale();
	scaling({ 1 / s.x, 1 / s.y, 1 / s.z });
	s.x = (next_length + prev_length) / 4;
	scaling(s);
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

		editional_update_func_(*obj);

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

void TrackNode::join_behave(Obj& obj, bool from_no_where)
{
	if (from_no_where)
	{
		cerr << "from noway" << endl;
	}
	if (VehicleObj* car = dynamic_cast<VehicleObj*>(&obj))
	{
		car->set_included_node(id_);
	}

	joined_objs_.push_back(&obj);
}

void TrackNode::detach_behave(Obj& obj, bool to_no_where)
{
	if (to_no_where)
	{
		// 복귀.
		cerr << "to noway" << endl;
	}
	else
	{
		cerr << "to nearnode" << endl;
	}

	joined_objs_.erase(std::remove(joined_objs_.begin(), joined_objs_.end(), &obj), joined_objs_.end());
}

void TrackNode::process_collide()
{
	process_collide(this);

	for (auto& prev : prev_nodes_)
	{
		process_collide(prev.get());
	}

	for (auto& next : next_nodes_)
	{
		process_collide(next.get());
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
	gui::Begin(("TrackNode::" + to_string(id_)).c_str());

	gui::Text("Prev_nodes");
	for (const auto& prev : prev_nodes_)
	{
		gui::InputInt("prev_ID", (int*)&prev->id_);
		// => rejoint
	}

	gui::Text("Next_nodes");
	for (const auto& next : next_nodes_)
	{
		gui::InputInt("next_ID", (int*)&next->id_);
		// => rejoint
	}

	gui::End();

	Obj::draw_gui();
}

void TrackNode::set_editional_update_func(obj_func func)
{
	editional_update_func_ = func; // throw error here;
};

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
	auto length = glm::length(get_scale()) * 0.3f;

	auto center = nodepos + dir * length + glm::vec3(0, 1, 0);
	auto rotate = glm::inverse(sunkueglm::quat_from2vectors(dir));
	auto scale = glm::vec3{ length, edgescale * 0.2f };

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
		draw_prev_edge(shader, prev.get());
	}

	for (auto& next : get_next_nodes())
	{
		draw_next_edge(shader, next.get());
	}
}

////////////////////////////////////////////////
Track::Track()
{
	load("track");
	set_start_node(tracks_.back());
	set_mid1_node(tracks_[10]);  // 변경
	set_mid2_node(tracks_[11]);  // 변경
	set_end_node(tracks_.front());
}

void Track::cacul_from_starts()
{
	// s= n, e = 0 // s.next,e.prev size = 1 //
	queue<TrackNodePtr> numbering_track_list;
	end_point_->from_start_ = 0;
	numbering_track_list.push(end_point_);
	do {
		for (auto& n : numbering_track_list.front()->next_nodes_)
		{
			n->from_start_ = std::max(n->from_start_, numbering_track_list.front()->from_start_ + 1);
			numbering_track_list.push(n);
		}
		numbering_track_list.pop();
	} while (end_point_ != numbering_track_list.front());
	end_point_->from_start_ = 0;

	cout << "caclued from_start" << endl;
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
				prev_ids.emplace_back(prev->id_);
			}
			SAVE_FILE(file, prev_ids);


			auto& nexts = node->get_next_nodes();
			vector<int> next_ids;
			for (const auto& next : nexts)
			{
				next_ids.emplace_back(next->id_);
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
					const auto& is_prev = std::find(ALLOF(slected_node->get_prev_nodes()), node);

					if (fail != is_prev)
					{
						node->draw(shader, Model::box_yellow()); // 노랑박스
						continue;
					}
				}

				{
					// check is node in slectedNode's nextlist.
					const auto& fail = slected_node->get_next_nodes().cend();
					const auto& is_next = std::find(cALLOF(slected_node->get_next_nodes()), node);

					if (fail != is_next)
					{
						node->draw(shader, Model::box_green()); // 초록박스
						continue;
					}
				}
			}
		}

		if (node == start_point_)
		{
			node->draw(shader, Model::box_purple()); 
			continue;
		}
		else if (node == mid_point1_)
		{
			node->draw(shader, Model::box_bludyred());
			continue;
		}
		else if (node == mid_point2_)
		{
			node->draw(shader, Model::box_redpurple()); 
			continue;
		}
		else if (node == end_point_)
		{
			node->draw(shader, Model::box_orange()); 
			continue;
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
	if (gui::Button("nn"))
	{
		cacul_from_starts();
	}
	if (gui::Button("update nodes"))
	{
		for (auto& node : tracks_)
		{
			node->update_front();
			node->update_rotate();
			node->update_scale();
		}
	}

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

void Track::set_start_node(TrackNodePtr& newNode)
{
	if (start_point_)
	{
		start_point_->set_editional_update_func();
	}
	newNode->set_editional_update_func(
		[](Obj& obj)
		{
		});
	start_point_ = newNode;
}

void Track::set_end_node(TrackNodePtr& newNode)
{
	if (end_point_)
	{
		end_point_->set_editional_update_func();
	}
	newNode->set_editional_update_func(
		[](Obj& obj)
		{
			if (auto car = dynamic_cast<VehicleObj*>(&obj))
			{
				auto prev_cp = car->get_check_point();
				auto pre_target = VehicleObj::CHECK_POINT::check2;

				if (pre_target == prev_cp)
				{
					car->clear_lab();
				}
				car->set_check_point(VehicleObj::CHECK_POINT::begin);
			}
		});
	end_point_ = newNode;
}

void Track::set_mid1_node(TrackNodePtr& newNode)
{
	if (mid_point1_)
	{
		mid_point1_->set_editional_update_func();
	}
	newNode->set_editional_update_func(
		[](Obj& obj)
		{
			if (auto car = dynamic_cast<VehicleObj*>(&obj))
			{
				car->set_check_point(VehicleObj::CHECK_POINT::check1);
			}
		});
	mid_point1_ = newNode;
}

void Track::set_mid2_node(TrackNodePtr& newNode)
{
	if (mid_point2_)
	{
		mid_point2_->set_editional_update_func();
	}
	newNode->set_editional_update_func(
		[](Obj& obj)
		{
			if (auto car = dynamic_cast<VehicleObj*>(&obj))
			{
				car->set_check_point(VehicleObj::CHECK_POINT::check2);
			}
		});
	mid_point2_ = newNode;
}


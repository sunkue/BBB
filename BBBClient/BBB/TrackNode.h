#pragma once

#include "Obj.h"


///////////////////////////////////////////////////////////////
// 
// template node<T> 를 맹글까?
// 
///////////////////////////////////////////////////////////////

class TrackNode : public Obj
{
	friend class Track;
public:
	explicit TrackNode(const ModelPtr& model) : Obj{ model } {};

public:
	void init()
	{
		// 모든 노드 연결 후,
		// front vector pre-cacul.

		return;

		const glm::vec3 pos = get_position();
		glm::vec3 front{ 0 };
		for (const auto& next : next_nodes_)
		{
			auto diff = next->get_position() - pos;
			front += diff;
		};
		front /= next_nodes_.size();

		//
	}

public:
	void update()
	{
		for (const auto& obj : joined_objs_)
		{
			if (false == check_include(*obj))
			{
				process_detach(*obj);
			}
		}
	}

	bool check_include(const Obj& obj)
	{
		obj.get_position();
		// intesect => point with node(box?).
		// process_detach();
	}

private:
	void process_detach(const Obj& obj)
	{
		// check_include(obj) with next, prev and join to on of those node.
	}

public:
	void process_collide() // 연결된 노드들 포함.
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

private:
	void process_collide(TrackNode* node)
	{
		for (const auto& obj : joined_objs_)
		{
			for (const auto& other : node->joined_objs_)
			{
				if (obj == other) // this 가 아니면 어차피 안하는 것...? 제거?
				{
					continue;
				}

				obj->collision_detect(*other); // instersect로 검사 후 액션.
			}
		}
	}
protected:
	void add_prev(TrackNode* prev, bool joint_them = true)
	{
		if (nullptr == prev) return;

		prev_nodes_.emplace_back(prev);
		if (joint_them)
		{
			prev->add_next(this, false);
		}
	}

	void add_next(TrackNode* next, bool joint_them = true)
	{
		if (nullptr == next) return;

		next_nodes_.emplace_back(next);
		if (joint_them)
		{
			next->add_prev(this, false);
		}
	}

	void draw_edges(const ShaderPtr& shader) const;
	void draw_prev_edge(const ShaderPtr& shader, TrackNode* prev) const;
	void draw_next_edge(const ShaderPtr& shader, TrackNode* next) const;

public:
	virtual void draw_gui() override;

	GET_REF(prev_nodes);
	GET_REF(next_nodes);

private:
	unsigned int id; 
	vector<TrackNode*> prev_nodes_;
	vector<TrackNode*> next_nodes_;
	
private:
	glm::vec3 front_; // position - position 으로 진행방향 구성
	//vector<vehicle*> include_objs_; 
	//vector<item*> include_objs_; 
	vector<ObjPtr> joined_objs_; // prev, next 와 함께 충돌검사. 

	// 겹칠경우 진행방향 쪽 노드에 포함(next node 인 노드에 포함).
	// 0 => 진행방향 => 1. 높을 수록 진행방향 쪽.
	// 
	// 이어지는 노드들 선형보간 텍스쳐 다닥다닥 곡선 트랙 맹글기.

	// 두 노드 사이를 끝으로 하는 사각형(연결엣지) 그리기? 
	// 몇개가 될지 모름. 반복문형식으로,,
	// 중심 에서 front 로 절반만 노랑,
	// front 에서 next의 중심으로 절반만 초록..? 

};

//////////////////////////////////////////////

class Track // : public IDataOnFile
{
	SINGLE_TON(Track)
	{
		unsigned int id = 0;

		auto model = Model::box(); // Model::no_model();
		glm::vec3 sp = { 206.311, 0, -108.274 };
		auto len = glm::length(sp);

		for (int i = 0, nodes = 60; i < nodes; i++)
		{
			glm::vec3 dir = glm::rotate(sp, glm::radians(360.f / nodes) * i, Y_DEFAULT);
			dir = glm::normalize(dir);
			tracks_.emplace_back(make_shared<TrackNode>(model));
			tracks_.back()->move(len * dir);
			tracks_.back()->id = id++;

			if (i > 0)
			{
				tracks_.back()->add_prev(tracks_[i - 1].get());
			}
		}
		tracks_.back()->add_next(tracks_[0].get());
		tracks_.back()->add_next(tracks_[1].get());
	}

protected:
	//	virtual void save_file_impl(ofstream& file) final;
	//	virtual void load_file_impl(ifstream& file) final;
private:
	void draw_edges(const ShaderPtr& shader) const;
		
public:
	void draw(const ShaderPtr& shader);
	void draw_gui();

public:
	GET_REF(tracks);
private:
	vector<shared_ptr<TrackNode>> tracks_;
	
	bool draw_all_edges_ = false;
	bool draw_nearby_edges_ = false;
	bool draw_select_edges_ = false;
	
};

///////////////////////////////////////////////////////////////

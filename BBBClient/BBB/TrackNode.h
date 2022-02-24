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
public:
	virtual void draw_gui() override;

	GET_REF(prev_nodes);
	GET_REF(next_nodes);

private:
	unsigned int id; // 노필요일지도.
	vector<TrackNode*> prev_nodes_;
	vector<TrackNode*> next_nodes_;

private:
	glm::vec3 front_; // position - position 으로 진행방향 구성
	//vector<vehicle*> include_objs_; 
	//vector<item*> include_objs_; 
	vector<Obj*> joined_objs_; // prev, next 와 함께 충돌검사. 
	
	// 겹칠경우 진행방향 쪽 노드에 포함(next node 인 노드에 포함).
	// 0 => 진행방향 => 1. 높을 수록 진행방향 쪽.
	// 
	// 이어지는 노드들 선형보간 텍스쳐 다닥다닥 곡선 트랙 맹글기.

};

///////////////////////////////////////////////////////////////
// 
// 트리형식으로 맹글어서 head == 0 -> 1 -> 2 -> 0  으로 연결? 
// vector<TrackNode*> next_nodes_; //=> next 및 prev 가 여러개일 수 있게 됨. 갈림길 표현.
// 
///////////////////////////////////////////////////////////////

class Track // : public IDataOnFile
{
	SINGLE_TON(Track)
	{
		unsigned int id = 0;

		tracks_.emplace_back(make_shared<TrackNode>(Model::box()));
//		tracks_.emplace_back(make_unique<TrackNode>(Model::no_model()));
		tracks_.back()->move({ 206.311, 0, -108.274 });
		tracks_.back()->id = id++;
	}

protected:
	//	virtual void save_file_impl(ofstream& file) final;
	//	virtual void load_file_impl(ifstream& file) final;

public:
	void draw(const ShaderPtr& shader)
	{
		for (auto& node : tracks_)
		{
			node->update_uniform_vars(shader);
			node->draw(shader);
		}
	}

	GET_REF(tracks);

private:
	vector<shared_ptr<TrackNode>> tracks_;
};

///////////////////////////////////////////////////////////////

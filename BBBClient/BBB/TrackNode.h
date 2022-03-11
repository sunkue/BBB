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

protected:
	virtual void save_file_impl(ofstream& file) final;
	virtual void load_file_impl(ifstream& file) final;

public:
	void update_front();

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

	bool check_include(Obj& obj)
	{
		bool isCollide = get_boundings().intersects(obj.get_boundings());
		return isCollide;
	}

	bool is_joined(Obj& obj)
	{
		auto res = std::find(ALLOF(joined_objs_), &obj);
		return (res != joined_objs_.end());
	}
private:
	void process_detach(Obj& obj)
	{
		for (auto& next : next_nodes_)
		{
			if (next->check_include(obj))
			{
				return detach_behave(obj);
			}
		}

		for (auto& prev : prev_nodes_)
		{
			if (prev->check_include(obj))
			{
				return detach_behave(obj);
			}
		}

		return detach_behave(obj, true);
	}

	void join_behave(Obj& obj, bool from_no_where = false)
	{
		if (from_no_where)
		{
			cerr << "from noway" << endl;
		}
	}

	void detach_behave(Obj& obj, bool to_no_where = false)
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

public:
	void process_collide(); // 연결된 노드들 포함.

private:
	void process_collide(TrackNode* node);

protected:
	void add_prev(TrackNode* prev, bool joint_them = true);
	void add_next(TrackNode* next, bool joint_them = true);

	void draw_edges(const ShaderPtr& shader) const;
	void draw_prev_edge(const ShaderPtr& shader, TrackNode* prev) const;
	void draw_next_edge(const ShaderPtr& shader, TrackNode* next) const;
	void draw_front_edge(const ShaderPtr& shader) const;

public:
	virtual void draw_gui() override;

	GET_REF(prev_nodes);
	GET_REF(next_nodes);

private:
	int id;
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

//////////////////////////////////////////////

class Track : public IDataOnFile
{
	SINGLE_TON(Track);

protected:
	virtual void save_file_impl(ofstream& file) final;
	virtual void load_file_impl(ifstream& file) final;

private:
	void draw_edges(const ShaderPtr& shader) const;

public:
	void draw(const ShaderPtr& shader);
	void draw_gui();

public:
	void update()
	{
		for (auto& node : tracks_)
		{
			node->update();
		}
	}
public:
	int check_include(Obj& obj) // return -1 to fail
	{
		for (auto& track : tracks_)
		{
			if (track->check_include(obj))
			{
				return track->id;
			}
		}

		return -1;
	}

	void include_obj(Obj& obj)
	{
		auto res = check_include(obj);

		if (-1 == res)
		{
			res = find_closest_track(obj);
		}

		tracks_[res]->joined_objs_.push_back(&obj);
		cerr << "near track ==[ " << res << " ]" << endl;
	}

	int find_closest_track(Obj& obj)
	{
		pair<int, float> ret = make_pair(-1, -1.f);

		auto pos = obj.get_position();

		for (auto& track : tracks_)
		{
			auto len = glm::length(track->get_position() - pos);

			if (ret.second < 0 || len < ret.second)
			{
				ret = make_pair(track->id, len);
			}
		}

		return ret.first;
	}
public:
	GET_REF(tracks);
private:
	vector<shared_ptr<TrackNode>> tracks_;

	bool draw_ = true;
	bool draw_all_edges_ = true;
	bool draw_nearby_edges_ = true;
	bool draw_select_edges_ = true;
	bool wiremode_ = false;

};

///////////////////////////////////////////////////////////////

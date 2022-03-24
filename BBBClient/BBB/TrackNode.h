#pragma once

#include "Obj.h"


using TrackNodePtr = shared_ptr<class TrackNode>;
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
	void update_rotate();
	void update_scale();

public:
	void update();

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
				detach_behave(obj);
				next->join_behave(obj);
				return;
			}
		}

		for (auto& prev : prev_nodes_)
		{
			if (prev->check_include(obj))
			{
				detach_behave(obj);
				prev->join_behave(obj);
				return;
			}
		}

		return detach_behave(obj, true);
	}
	void join_behave(Obj& obj, bool from_no_where = false);
	void detach_behave(Obj& obj, bool to_no_where = false);

public:
	glm::vec3 get_next_center()
	{
		glm::vec3 ret{ 0 };
		for (auto& next : next_nodes_)
		{
			ret += next->get_position();
		}
		ret /= next_nodes_.size();
		return ret;
	}
	glm::vec3 get_prev_center()
	{
		glm::vec3 ret{ 0 };
		for (auto& prev : prev_nodes_)
		{
			ret += prev->get_position();
		}
		ret /= prev_nodes_.size();
		return ret;
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
	GET(id);
	GET(from_start);
private:
	int id_; 
	int from_start_{}; //=> 작을 수록 출발점에 가까운...
	vector<TrackNodePtr> prev_nodes_;
	vector<TrackNodePtr> next_nodes_;

private:
	glm::vec3 front_; // position - position 으로 진행방향 구성
	//vector<vehicle*> include_objs_; 
	//vector<item*> include_objs_; 
	vector<Obj*> joined_objs_; // prev, next 와 함께 충돌검사. 

	// 겹칠경우 진행방향 쪽 노드에 포함(next node 인 노드에 포함).
	// 0 => 진행방향 => 1. 높을 수록 진행방향 쪽.
	// 
	// 이어지는 노드들 선형보간 텍스쳐 다닥다닥 곡선 트랙 맹글기.
	using obj_func = std::function<void(Obj&)>;

public:
	void set_editional_update_func(obj_func func = [](Obj&) {});
private:
	// 스타트, 엔드, 중간노드들.
	obj_func editional_update_func_{ [](Obj&) {} };
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
	int check_include(Obj& obj) // return -1 on fail
	{
		for (auto& track : tracks_)
		{
			if (track->check_include(obj))
			{
				return track->id_;
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
		tracks_[res]->join_behave(obj);
		cerr << "near track ==[ " << res << " ]" << endl;
	}

	int find_closest_track(Obj& obj) //return -1 on fail.
	{
		pair<int, float> ret = make_pair(-1, -1.f);

		auto pos = obj.get_position();

		for (auto& track : tracks_)
		{
			auto len = glm::length(track->get_position() - pos);

			if (ret.second < 0 || len < ret.second)
			{
				ret = make_pair(track->id_, len);
			}
		}

		return ret.first;
	}

public:
	GET_REF(tracks);
private:
	vector<TrackNodePtr> tracks_;

private:
	void set_start_node(TrackNodePtr& newNode);
	void set_end_node(TrackNodePtr& newNode);
	void set_mid1_node(TrackNodePtr& newNode);
	void set_mid2_node(TrackNodePtr& newNode);
	
	void cacul_from_starts();

private:
	TrackNodePtr start_point_;
	TrackNodePtr mid_point1_;
	TrackNodePtr mid_point2_;
	TrackNodePtr end_point_;

private:
	// check_point => 비교, 업데이트에서 포함 오브제중 차량에 적용,,
	// 차량에 체크포인트 변수,, lab 변수,, 
	// lab수 + end point 까지 남은 수 + front 로 판단,, 
	// end point 는 start node 다음,, 두 노드 경계가 출발선.
	// s->e->m1->m2->s->e // s->m1 // m1->m2 // m2->e(+1) // m2->m1(reverse) 

	bool draw_ = true;
	bool draw_all_edges_ = true;
	bool draw_nearby_edges_ = true;
	bool draw_select_edges_ = true;
	bool wiremode_ = false;

};

///////////////////////////////////////////////////////////////

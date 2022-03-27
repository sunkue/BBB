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
	void process_collide(); // ����� ���� ����.

private:
	void process_collide(TrackNode* node);

protected:
	void add_prev(TrackNode* prev, bool joint_them = true);
	void add_next(TrackNode* next, bool joint_them = true);
	static void unjoint(TrackNode* prev, TrackNode* next);

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
	GET(front);
private:
	int id_;
	int from_start_{}; //=> ���� ���� ��ŸƮ���ο� �����...
	vector<TrackNode*> prev_nodes_;
	vector<TrackNode*> next_nodes_;

private:
	glm::vec3 front_;
	//vector<vehicle*> include_objs_; 
	//vector<item*> include_objs_; 
	vector<Obj*> joined_objs_; // prev, next �� �Բ� �浹�˻�. 

	// ��ĥ��� ������� �� ��忡 ����(next node �� ��忡 ����).
	// 0 => ������� => 1. ���� ���� ������� ��.
	// 
	// �̾����� ���� �������� �ؽ��� �ٴڴٴ� � Ʈ�� �ͱ۱�.
	using obj_func = std::function<void(Obj&)>;

public:
	void set_editional_update_func(obj_func func = [](Obj&) {});
private:
	// ��ŸƮ, ����, �߰�����.
	obj_func editional_update_func_{ [](Obj&) {} };
};
//////////////////////////////////////////////

class Track : public IDataOnFile
{
	using outlanded_obj = pair<milliseconds, Obj*>;
	using outlanded_update_func = std::function<void(outlanded_obj&)>;

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
	outlanded_update_func outlanded_update_func_{};

	void update_outlanded_obj(outlanded_obj& outlanded_obj)
	{
		outlanded_update_func_(outlanded_obj);
	}

	void update();

public:
	// return -1 on fail
	int check_include(Obj& obj)
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

	void init_include_obj(Obj& obj, bool from_no_where)
	{
		auto res = check_include(obj);

		if (-1 == res)
		{
			return;
			/// res = find_closest_track(obj);
		}
		tracks_[res]->join_behave(obj, from_no_where);
		cerr << "included track ==[ " << res << " ]" << endl;
	}

	void include_obj(Obj& obj, bool from_no_where, glm::vec3 diff = glm::vec3{ 0 })
	{
		auto res = check_include(obj);

		if (-1 == res)
		{
			res = find_closest_track(obj);
		}

		auto move = tracks_[res]->get_position() - obj.get_position() + diff; move.y = 0;
		obj.move(move);
		tracks_[res]->join_behave(obj, from_no_where);
		cerr << "included track ==[ " << res << " ]" << endl;
	}

	//return -1 on fail.
	int find_closest_track(Obj& obj)
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
	GET_REF_UNSAFE(objs_in_outland);
private:
	vector<TrackNodePtr> tracks_;
	vector<outlanded_obj> objs_in_outland_;

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
	bool draw_ = true;
	bool draw_all_edges_ = true;
	bool draw_nearby_edges_ = true;
	bool draw_select_edges_ = true;
	bool wiremode_ = false;

};

///////////////////////////////////////////////////////////////

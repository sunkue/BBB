#pragma once

#include "Obj.h"


///////////////////////////////////////////////////////////////
// 
// template node<T> �� �ͱ۱�?
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
		// ��� ��� ���� ��,
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
	void process_collide() // ����� ���� ����.
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
				if (obj == other) // this �� �ƴϸ� ������ ���ϴ� ��...? ����?
				{
					continue;
				}

				obj->collision_detect(*other); // instersect�� �˻� �� �׼�.
			}
		}
	}
public:
	virtual void draw_gui() override;

	GET_REF(prev_nodes);
	GET_REF(next_nodes);

private:
	unsigned int id; // ���ʿ�������.
	vector<TrackNode*> prev_nodes_;
	vector<TrackNode*> next_nodes_;

private:
	glm::vec3 front_; // position - position ���� ������� ����
	//vector<vehicle*> include_objs_; 
	//vector<item*> include_objs_; 
	vector<Obj*> joined_objs_; // prev, next �� �Բ� �浹�˻�. 
	
	// ��ĥ��� ������� �� ��忡 ����(next node �� ��忡 ����).
	// 0 => ������� => 1. ���� ���� ������� ��.
	// 
	// �̾����� ���� �������� �ؽ��� �ٴڴٴ� � Ʈ�� �ͱ۱�.

};

///////////////////////////////////////////////////////////////
// 
// Ʈ���������� �ͱ۾ head == 0 -> 1 -> 2 -> 0  ���� ����? 
// vector<TrackNode*> next_nodes_; //=> next �� prev �� �������� �� �ְ� ��. ������ ǥ��.
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

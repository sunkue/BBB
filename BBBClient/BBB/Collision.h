#pragma once

#include "FileHelper.h"

// 구, 사각형, 레이, 프리스텀

struct Ray
{
	glm::vec3 origin{};
	glm::vec3 dir{};

	static Ray create(int wx, int wy);
};


struct BoundingSphere
{
	glm::vec3 center{};
	float radius{ 1 };

	bool intersects(const struct BoundingSphere& sh) const;
	//	bool intersects(const struct BoundingBox& box) const;
	bool intersects(const struct BoundingFrustum& fr) const;
	bool intersects(const struct Ray& ray)const;
};

struct BoundingBox
{
	glm::vec3 center{};
	glm::vec3 extents{ 1 };
	glm::quat orientation{};

	//	bool intersects(const struct BoundingSphere& sh) const;
	bool intersects(const struct BoundingBox& box) const;
	bool intersects(const struct BoundingFrustum& fr) const;
	bool intersects(const struct Ray& ray, float& dist)const;
private:
	bool getSeparatingPlane(const glm::vec3& RPos, const glm::vec3& Plane, const BoundingBox& box) const;
	glm::vec3 AxisX()const { return glm::rotate(orientation, X_DEFAULT); }
	glm::vec3 AxisY()const { return glm::rotate(orientation, Y_DEFAULT); }
	glm::vec3 AxisZ()const { return glm::rotate(orientation, Z_DEFAULT); }
};


struct BoundingFrustum
{
	glm::vec3 origin{};
	glm::quat orientation{};

	float right_slope;
	float left_slope;
	float top_slope;
	float bottom_slope;
	float n, f;

	bool intersects(const struct BoundingSphere& sh) const;
	bool intersects(const struct BoundingBox& box) const;
	bool intersects(const struct BoundingFrustum& fr) const;
	bool intersects(const struct Ray& ray)const;

	static void CreateFromMatrix(glm::mat4 projection);
};

///////////////////////////////////

class Boundings : public IDataOnFile
{
protected:
	virtual void save_file_impl(ofstream& file) final;
	virtual void load_file_impl(ifstream& file) final;

	// for gui
private:
	BoundingSphere L1_current;
	BoundingSphere L1_prev;
	BoundingBox L2_current;
	BoundingBox L2_prev;

public:
	// L1 should contain L2.
	BoundingSphere L1;
	BoundingBox L2;


	ModelPtr sphere{ Model::sphere() };
	ModelPtr box{ Model::box() };

	ShaderPtr bounding_shader{ Shader::basic() };

	void rotate(const glm::quat& q) { L2.orientation *= q; }
	void move(const glm::vec3& dif) { L1.center += dif; L2.center += dif; }
	void scaling(const glm::vec3& ratio) { L2.extents *= ratio; L1.radius *= glm::compMax(ratio); }

	bool intersects(const Ray& ray, float& dist) const
	{
		return L2.intersects(ray, dist);
	}

	bool intersects(const Boundings& other) const
	{
		if (L1.intersects(other.L1))
		{
			return L2.intersects(other.L2);
		}

		return false;
	}

private:
	bool L1_on = false;
	bool L2_on = false;
public:
	void trans_debug()
	{
		L1.center += L1_current.center - L1_prev.center;
		L1_prev.center = L1_current.center;
		L1.radius *= L1_current.radius / L1_prev.radius;
		L1_prev.radius = L1_current.radius;

		L2.center += L2_current.center - L2_prev.center;
		L2_prev.center = L2_current.center;
		L2.extents *= L2_current.extents / L2_prev.extents;
		L2_prev.extents = L2_current.extents;
		L2.orientation *= glm::inverse(L2_prev.orientation);
		L2.orientation *= L2_current.orientation;
		L2_prev.orientation = L2_current.orientation;
	}

	void draw_gui()
	{
		gui::Begin("Boundings");
		gui::Text("This is Boundings in real game.");

		GUISAVE(); GUILOAD();

		gui::Text("L1, Shpere");
		gui::Checkbox("L1 show", &L1_on);
		if (gui::DragFloat3("L1 center", glm::value_ptr(L1_current.center), 0.0625, 0.1, 20, NULL, 1))
		{
			L1.center += L1_current.center - L1_prev.center;
			L1_prev.center = L1_current.center;
		}
		if (gui::SliderFloat("L1 radius", &L1_current.radius, 0.0625, 10.0f))
		{
			L1.radius *= L1_current.radius / L1_prev.radius;
			L1_prev.radius = L1_current.radius;
		}

		gui::Text("L2, Box");
		gui::Checkbox("L2 show", &L2_on);
		if (gui::DragFloat3("L2 center", glm::value_ptr(L2_current.center), 0.0625, 0.1, 20, NULL, 1))
		{
			L2.center += L2_current.center - L2_prev.center;
			L2_prev.center = L2_current.center;
		}
		if (gui::DragFloat3("L2 extents", glm::value_ptr(L2_current.extents), 0.0625, 0.1, 20, NULL, 1))
		{
			L2.extents *= L2_current.extents / L2_prev.extents;
			L2_prev.extents = L2_current.extents;
		}
		if (gui::DragFloat4("L2 orientation", glm::value_ptr(L2_current.orientation), 0.0625))
		{
			L2_current.orientation = glm::normalize(L2_current.orientation);
			L2.orientation *= glm::inverse(L2_prev.orientation);
			L2.orientation *= L2_current.orientation;
			L2_prev.orientation = L2_current.orientation;
		}

		gui::End();
	}

	void draw()const
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (L1_on)
		{
			bounding_shader->use();
			glm::mat4 m = glm::translate(L1.center) * glm::scale(glm::vec3{ L1.radius });
			bounding_shader->set("u_m_mat", m);
			sphere->draw(bounding_shader);
			glUseProgram(0);
		}

		if (L2_on)
		{
			bounding_shader->use();
			glm::mat4 m = glm::translate(L2.center) * glm::toMat4(L2.orientation) * glm::scale(L2.extents);
			bounding_shader->set("u_m_mat", m);
			box->draw(bounding_shader);
			glUseProgram(0);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
};
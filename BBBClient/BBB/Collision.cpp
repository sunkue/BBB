#include "stdafx.h"
#include "game.h"
#include "Collision.h"

////////////////////////////////////////////////////

Ray Ray::create(int wx, int wy)
{
	int w = screen.width;
	int h = screen.height;

	const auto& renderer = Game::get().renderer;
	const auto* camera = renderer.get_main_camera().get();
	
	const auto view_matrix = camera->view_mat();
	const auto proj_mat = renderer.proj_mat();

	float x = (((2.f * wx) / w) - 1.f);
	float y = (1.f - (2.f * wy) / h);
	float z = 1.0f;

	glm::vec3 ray_nds{ x, y, z };
	glm::vec4 ray_clip = { ray_nds.x, ray_nds.y, -1.f, 1.f };

	glm::vec4 ray_eye = inverse(proj_mat) * ray_clip;
	ray_eye = { ray_eye.x, ray_eye.y, -1.f, 0.f };

	glm::vec3 ray_wor = inverse(view_matrix) * ray_eye;
	ray_wor = glm::normalize(ray_wor);

	Ray ret;
	ret.origin = camera->get_position();
	ret.dir = ray_wor;
	return ret;
}



////////////////////////////////////////////////////

bool BoundingSphere::intersects(const BoundingSphere& sh) const
{
	auto dist2 = glm::distance2(center, sh.center);
	auto radius2 = radius + sh.radius; radius2 *= radius2;
	return dist2 <= radius2;
}

/*
bool BoundingSphere::intersects(const BoundingBox& box) const
{
	return false;
}
*/

bool BoundingSphere::intersects(const BoundingFrustum& fr) const
{
	return false;
}

bool BoundingSphere::intersects(const Ray& ray) const
{
	/*
	float t0, t1; // solutions for t if the ray intersects
#if 0
		// geometric solution
	Vec3f L = center - orig;
	float tca = L.dotProduct(dir);
	// if (tca < 0) return false;
	float d2 = L.dotProduct(L) - tca * tca;
	if (d2 > radius2) return false;
	float thc = sqrt(radius2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;
#else
		// analytic solution
	glm::vec3 L = orig - center;
	float a = dir.dotProduct(dir);
	float b = 2 * dir.dotProduct(L);
	float c = L.dotProduct(L) - radius2;
	if (!solveQuadratic(a, b, c, t0, t1)) return false;
#endif
	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0) {
		t0 = t1; // if t0 is negative, let's use t1 instead
		if (t0 < 0) return false; // both t0 and t1 are negative
	}

	t = t0;

	return true;
	*/
	return false;
}

////////////////////////////////////////////////////

/*
bool BoundingBox::intersects(const BoundingSphere& sh) const
{
	auto SphereCenter = sh.center;
	auto SphereRadius = sh.radius;

	auto BoxCenter = center;
	auto BoxExtents = extents;
	auto BoxOrientation = orientation;

	// Transform the center of the sphere to be local to the box.
	// BoxMin = -BoxExtents
	// BoxMax = +BoxExtents
	SphereCenter = glm::rotate(glm::inverse(BoxOrientation), SphereCenter - BoxCenter);


	// Find the distance to the nearest point on the box.
	// for each i in (x, y, z)
	// if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
	// else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2
	auto d = glm::vec3(0);

	// Compute d for each dimension.
	auto LessThanMin = glm::lessThan(SphereCenter, -BoxExtents);
	auto GreaterThanMax = glm::greaterThan(SphereCenter, BoxExtents);

	auto MinDelta = SphereCenter + BoxExtents;
	auto MaxDelta = SphereCenter - BoxExtents;

	// Choose value for each dimension based on the comparison.
	d = XMVectorSelect(d, MinDelta, LessThanMin);
	d = XMVectorSelect(d, MaxDelta, GreaterThanMax);
	glm::intersectLineSphere();
	// Use a dot-product to square them and sum them together.
	auto d2 = glm::dot(d, d);

	return XMVector4LessOrEqual(d2, XMVectorMultiply(SphereRadius, SphereRadius)) ? true : false;

}
*/

// check if there's a separating plane in between the selected axes
bool BoundingBox::getSeparatingPlane(const glm::vec3& RPos, const glm::vec3& Plane, const BoundingBox& box) const
{
	return fabs(glm::dot(RPos, Plane)) > (
		fabs(glm::dot(AxisX() * extents.x, Plane)) +
		fabs(glm::dot(AxisY() * extents.y, Plane)) +
		fabs(glm::dot(AxisZ() * extents.z, Plane)) +
		fabs(glm::dot(box.AxisX() * box.extents.x, Plane)) +
		fabs(glm::dot(box.AxisY() * box.extents.y, Plane)) +
		fabs(glm::dot(box.AxisZ() * box.extents.z, Plane)));
}



bool BoundingBox::intersects(const BoundingBox& box) const
{
	auto RPos = center - box.center;

	return !(
		getSeparatingPlane(RPos, AxisX(), box) ||
		getSeparatingPlane(RPos, AxisY(), box) ||
		getSeparatingPlane(RPos, AxisZ(), box) ||
		getSeparatingPlane(RPos, box.AxisX(), box) ||
		getSeparatingPlane(RPos, box.AxisY(), box) ||
		getSeparatingPlane(RPos, box.AxisZ(), box) ||
		getSeparatingPlane(RPos, glm::cross(AxisX(), box.AxisX()), box) ||
		getSeparatingPlane(RPos, glm::cross(AxisX(), box.AxisY()), box) ||
		getSeparatingPlane(RPos, glm::cross(AxisX(), box.AxisZ()), box) ||
		getSeparatingPlane(RPos, glm::cross(AxisY(), box.AxisX()), box) ||
		getSeparatingPlane(RPos, glm::cross(AxisY(), box.AxisY()), box) ||
		getSeparatingPlane(RPos, glm::cross(AxisY(), box.AxisZ()), box) ||
		getSeparatingPlane(RPos, glm::cross(AxisZ(), box.AxisX()), box) ||
		getSeparatingPlane(RPos, glm::cross(AxisZ(), box.AxisY()), box) ||
		getSeparatingPlane(RPos, glm::cross(AxisZ(), box.AxisZ()), box));
}

bool BoundingBox::intersects(const BoundingFrustum& fr) const
{
	return false;
}

bool BoundingBox::intersects(const Ray& ray, float& dist) const
{
	constexpr float epsilon = std::numeric_limits<float>::epsilon();

	dist = 0;

	auto delta = center - ray.origin;

	float tMin = numeric_limits<float>::min();
	float tMax = numeric_limits<float>::max();

	//x
	{
		auto xAxis = AxisX();
		float e = glm::dot(xAxis, delta);
		float f = glm::dot(xAxis, ray.dir);

		//if (e < fabs(f))
		{
			float t1 = (e + extents.x) / f;
			float t2 = (e - extents.x) / f;
			if (t2 < t1)swap(t1, t2);
			if (tMin < t1)tMin = t1;
			if (t2 < tMax)tMax = t2;
			if (tMax < tMin)return false;
			if (tMax < epsilon)return false;
		}
		//else if (0 < -e - extents.x || -e + extents.x < 0)return false;
	}

	//y
	{
		auto yAxis = AxisY();
		float e = glm::dot(yAxis, delta);
		float f = glm::dot(yAxis, ray.dir);

		//if (e < fabs(f))
		{
			float t1 = (e + extents.y) / f;
			float t2 = (e - extents.y) / f;
			if (t2 < t1)swap(t1, t2);
			if (tMin < t1)tMin = t1;
			if (t2 < tMax)tMax = t2;
			if (tMax < tMin)return false;
			if (tMax < epsilon)return false;
		}
		//	else if (0 < -e - extents.y || -e + extents.y < 0)return false;
	}

	//z
	{
		auto zAxis = AxisZ();
		float e = glm::dot(zAxis, delta);
		float f = glm::dot(zAxis, ray.dir);

		//	if (e < fabs(f))
		{
			float t1 = (e + extents.z) / f;
			float t2 = (e - extents.z) / f;
			if (t2 < t1)swap(t1, t2);
			if (tMin < t1)tMin = t1;
			if (t2 < tMax)tMax = t2;
			if (tMax < tMin)return false;
			if (tMax < epsilon)return false;
		}
		//	else if (0 < -e - extents.z || -e + extents.z < 0)return false;
	}

	// intersect.
	dist = (epsilon < tMin) ? tMin : tMax;
	return true;
}

////////////////////////////////////////////////////

bool BoundingFrustum::intersects(const BoundingSphere& sh) const
{
	return false;
}

bool BoundingFrustum::intersects(const BoundingBox& box) const
{
	return false;
}

bool BoundingFrustum::intersects(const BoundingFrustum& fr) const
{
	return false;
}

bool BoundingFrustum::intersects(const Ray& ray) const
{
	return false;
}

void BoundingFrustum::CreateFromMatrix(glm::mat4 projection)
{
}

////////////////////////////////////////////////////

void Boundings::load_file_impl(ifstream& file)
{
	LOAD_FILE(file, L1.center);
	LOAD_FILE(file, L1.radius);

	LOAD_FILE(file, L2.center);
	LOAD_FILE(file, L2.extents);
	LOAD_FILE(file, L2.orientation);
}

void Boundings::save_file_impl(ofstream& file)
{
	SAVE_FILE(file, L1.center);
	SAVE_FILE(file, L1.radius);

	SAVE_FILE(file, L2.center);
	SAVE_FILE(file, L2.extents);
	SAVE_FILE(file, L2.orientation);
}
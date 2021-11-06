#pragma once






using MaterialPtr = shared_ptr<struct Material>;

struct Material
{
	glm::vec3 ambient{ 1 };
	glm::vec3 diffuse{ 1, 1, .5f };
	glm::vec3 specular{ 0, 0, 1.f };
	float shininess{ 32.f };

	CREATE_SHARED(MaterialPtr, Material);
private:
	explicit Material() noexcept = default;
};



using LightPtr = shared_ptr<struct Light>;

struct Light
{
	glm::vec3 position{ 10.f,5.f,2.f };
	glm::vec3 ambient{ .2f };
	glm::vec3 diffuse{ .5f };
	glm::vec3 specular{ 1.f };
	
	CREATE_SHARED(LightPtr, Light);
private:
	explicit Light() noexcept = default;
};


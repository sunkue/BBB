#pragma once




/// //////////////////////////////////////

using MaterialPtr = shared_ptr<struct Material>;

struct Material
{
	Texture albedo;
	Texture specular;
	float shininess;

	CREATE_SHARED(MaterialPtr, Material);
private:
	Material(Texture albedoTex, Texture specularTex, float shininess = 32.f) noexcept
		: albedo{ albedoTex }, specular{ specularTex }, shininess{ shininess }{};
};

/// //////////////////////////////////////


struct LightBasic
{
	glm::vec3 ambient{ .2f };
	glm::vec3 diffuse{ .5f };
	glm::vec3 specular{ 1.f };
};


using DirectionalLightPtr = shared_ptr<struct DirectionalLight>;

struct DirectionalLight : LightBasic
{
	glm::vec3 direction{ 10.f,5.f,2.f };

	CREATE_SHARED(DirectionalLightPtr, DirectionalLight);
private:
	explicit DirectionalLight() noexcept = default;
};


using PointLightPtr = shared_ptr<struct PointLight>;

struct PointLight : LightBasic
{
	glm::vec3 position{ 10.f,5.f,2.f };
	glm::vec3 attenuation{ 1 ,0.0022f, 0.0019f };

	CREATE_SHARED(PointLightPtr, PointLight);
private:
	explicit PointLight() noexcept = default;
};


using SpotLightPtr = shared_ptr<struct SpotLight>;

struct SpotLight : LightBasic
{
	glm::vec3 position{ 10.f,5.f,2.f };
	glm::vec3 direction{ 0, -1, 0 };
	glm::vec3 attenuation{ 1 ,0.0022f, 0.0019f };
	float in_cutoff{ glm::cos(glm::radians(20.f)) };
	float out_cutoff{ glm::cos(glm::radians(25.f)) };

	CREATE_SHARED(SpotLightPtr, SpotLight);
private:
	explicit SpotLight() noexcept = default;
};

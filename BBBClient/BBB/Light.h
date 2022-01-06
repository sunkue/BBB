#pragma once




/// //////////////////////////////////////


struct LightBasic
{
	glm::vec3 ambient{ .1f };
	glm::vec3 diffuse{ .5f };
	glm::vec3 specular{ 1.f };
};


using DirectionalLightPtr = shared_ptr<struct DirectionalLight>;
struct DirectionalLight : LightBasic
{
	//glm::vec3 direction{ glm::normalize(glm::vec3(-15.f,-27.f,14.f)) };

	glm::vec3 direction{ glm::normalize(glm::vec3(-0.14,-0.2,1.f)) };
	//glm::vec3 direction{ glm::normalize(glm::vec3(1,0,0)) };

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
	friend class Shader;
public:
	glm::vec3 position{ 10.f,5.f,2.f };
	glm::vec3 direction{ 0, -1, 0 };
	glm::vec3 attenuation{ 1 ,0.0022f, 0.0019f };
private:
	float in_cutoff{ glm::cos(glm::radians(20.f)) };
	float out_cutoff{ glm::cos(glm::radians(25.f)) };
public:
	void set_in_cutoff(float angle) { in_cutoff = glm::cos(glm::radians(angle)); }
	void set_out_cutoff(float angle) { out_cutoff = glm::cos(glm::radians(angle)); }

	CREATE_SHARED(SpotLightPtr, SpotLight);
private:
	explicit SpotLight() noexcept = default;
};

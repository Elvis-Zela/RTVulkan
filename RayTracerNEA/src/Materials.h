#pragma once

#include <glm/glm.hpp>

enum matType { kReflective, kMetal, kLambertian, kDielectric };

struct Material
{
	glm::vec3 Albedo = glm::vec3(0.18f);

	float Roughness = 0.25f;
	float Fuzz = 0.15f;
	float IndexOfRefraction = 1.0f;

	matType type = kLambertian;
};


namespace MatUtils 
{
	float fresnel(const glm::vec3& incidence, const glm::vec3& normal, float indexOfRefraction);
	glm::vec3 refract(const glm::vec3& incidence, const glm::vec3& normal, float indexOfRefraction);
	glm::vec3 reflect(const glm::vec3& incidence, const glm::vec3& normal);
}


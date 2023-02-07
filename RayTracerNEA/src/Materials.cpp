#include "Materials.h"
#include "Utilities.h"

float MatUtils::fresnel(const glm::vec3& incidence, const glm::vec3& normal, float indexOfRefraction)
{
	float kr;
	float cosI = glm::clamp(dot(incidence, normal), -1.0f, 1.0f);

	float etai, etat;
	if (cosI < 0) { etai = 1, etat = indexOfRefraction; }
	else { etai = indexOfRefraction, etat = 1; }

	float sinT = etai / etat * sqrtf(std::max(0.0f, 1 - (cosI * cosI)));

	if (sinT >= 1) { kr = 1; }
	else
	{
		float cosT = sqrtf(std::max(0.0f, 1 - (sinT * sinT)));
		cosI = fabsf(cosI);
		float Rs = ((etat * cosI) - (etai * cosT)) / ((etat * cosI) + (etai * cosT));
		float Rp = ((etat * cosI) - (etai * cosT)) / ((etat * cosI) + (etat * cosT));
		kr = (Rs * Rs + Rp * Rp) * 0.5f;
	}

	return kr;
}

glm::vec3 MatUtils::reflect(const glm::vec3& incidence, const glm::vec3& normal)
{
	return incidence - 2 * glm::dot(incidence, normal) * normal;
}

glm::vec3 MatUtils::refract(const glm::vec3& incidence, const glm::vec3& normal, float indexOfRefraction)
{
	float cosI = glm::clamp(dot(incidence, normal), -1.0f, 1.0f);
	float etai = 1.0f, etat = indexOfRefraction;
	glm::vec3 n = normal;

	if (cosI < 0) { cosI = -cosI; }
	else
	{
		etai = indexOfRefraction;
		etat = 1.0f;
		n = -normal;
	}

	float eta = etai / etat;
	float k = 1 - (eta * eta) * (1 - (cosI * cosI));
	return k < 0 ? glm::vec3(0) : eta * incidence + (eta * cosI - sqrt(k)) * n;
}

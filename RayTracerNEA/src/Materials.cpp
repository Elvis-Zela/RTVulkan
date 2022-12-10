#include "Materials.h"

#include "Walnut/Random.h"

#include "Utilities.h"
#include "Ray.h"
#include "Hittables.h"

bool RoughMat::Scatter(const Ray& rayIn, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const
{
	glm::vec3 scatterDirection = payload.hitNormal + m_Roughness * Walnut::Random::Vec3(-0.5f, 0.5f);
	
	scatterDirection = glm::reflect(rayIn.GetRayDirection(), scatterDirection);
	scattered = Ray(payload.hitPoint + payload.hitNormal * NEAR_EPSILON, scatterDirection);
	attenuation = m_Albedo;
	return true;
}

bool Metal::Scatter(const Ray& rayIn, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const
{
	glm::vec3 reflected = glm::reflect(glm::normalize(rayIn.GetRayDirection()), payload.hitNormal);
	scattered = Ray(payload.hitPoint, reflected + m_Fuzz * Walnut::Random::InUnitSphere());
	attenuation = m_Albedo;
	return (glm::dot(scattered.GetRayDirection(), payload.hitNormal) > 0);
}

bool Lambertian::Scatter(const Ray& rayIn, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const
{
	glm::vec3 scatterDirection = payload.hitNormal + Walnut::Random::InUnitSphere();

	if (Utilities::nearZero(scatterDirection))
		scatterDirection = payload.hitNormal;

	scattered = Ray(payload.hitPoint + payload.hitNormal * NEAR_EPSILON, scatterDirection);
	attenuation = m_Albedo;
	return true;
}

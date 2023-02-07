#pragma once

#include <glm/glm.hpp>

enum rayType { kPrimaryRay, kShadowRay };

class Ray
{
public:
	/* - Constructors - */
	Ray() {}
	Ray(const glm::vec3& origin, const glm::vec3& direction);

	/* - Getters - */
	glm::vec3 GetRayOrigin()		const { return m_Origin; }
	glm::vec3 GetRayDirection()		const { return m_Direction; }

	void SetRayOrigin(glm::vec3 origin);
	void SetRayDirection(glm::vec3 direction);

	/* - Point on the ray given a t value - */
	glm::vec3 At(float t) const;

private:
	glm::vec3 m_Origin{ 0.0f };
	glm::vec3 m_Direction{ 0.0f };
};

class Sphere;

struct RayPayload
{
	float closestT;			// Holds current closest t value
	Sphere* object;			// Pointer to object
};

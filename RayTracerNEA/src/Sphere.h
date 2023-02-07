#pragma once

#include <glm/glm.hpp>

#include "Materials.h"
#include "Ray.h"

class Sphere
{
public:
	/* - Constructors - */
	Sphere(Material material, glm::vec3 center = glm::vec3(0.0f), float r = 0.5f);
	~Sphere();

	/* - Methods - */
	bool hit(const Ray& ray, float t_min, float t_max, RayPayload& payload, rayType rt = kPrimaryRay) const;
	glm::vec3 getNormal(const glm::vec3& hitPoint) const;

public:
	Material  mat;

public:
	/* - Attributes - */
	glm::vec3 m_Center;
	float	  m_Radius;
};

#pragma once
#include "Utilities.h"
#include <glm/glm.hpp>

#include "Hittables.h"

class Ray;

class Sphere : public Hittables
{
public:
	/* - Constructors - */
	Sphere(glm::vec3 center, float r, glm::vec3 albedo);
	~Sphere();

	/* - Methods - */
	virtual bool hit(const Ray& ray, float t_min, float t_max, hit_record& rec) const override;

private:
	/* - Attributes - */
	glm::vec3 m_Albedo;
	glm::vec3 m_Center;
	float	  m_Radius;
};
#pragma once

#include <glm/glm.hpp>

#include "Hittables.h"

class Ray;
class Material;

class Sphere : public Hittables
{
public:
	/* - Constructors - */
	Sphere(glm::vec3 center, float r, int matIndex = 0);
	~Sphere();

	/* - Methods - */
	virtual bool hit(const Ray& ray, float t_min, float t_max, RayPayload& rec) const override;
	virtual void ClosestHitShader(const Ray& ray, RayPayload& rec) const override;
	virtual int GetMatIndex() const override { return m_matIndex; }

private:
	/* - Attributes - */
	glm::vec3 m_Center;
	float	  m_Radius;
	int	  m_matIndex;
};

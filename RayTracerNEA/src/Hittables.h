#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Ray.h"

struct RayPayload
{
	glm::vec3 hitPoint;
	glm::vec3 hitNormal;

	float closestT;		// Holds current closest t value
	int objIdx;
};

class Hittables
{
public:
	virtual bool hit(const Ray& ray, float t_min, float t_max, RayPayload& rec) const = 0;
	virtual void ClosestHitShader(const Ray& ray, RayPayload& rec) const = 0;
	virtual int GetMatIndex() const = 0;
};

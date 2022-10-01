#pragma once

#include "Utilities.h"
#include <glm/glm.hpp>

#include "Ray.h"



struct RayPayload
{
	glm::vec3 hitPoint;
	glm::vec3 hitNormal;
	glm::vec3 hitAlbedo;		// Hit object intrinsic colour

	float closestT;			// Holds current closest t value
	int objectSceneIndex;
};

class Hittables
{
public:
	virtual bool hit(const Ray& ray, float t_min, float t_max, RayPayload& rec) const = 0;
	virtual void ClosestHitShader(const Ray& ray, RayPayload& rec) const = 0;
};

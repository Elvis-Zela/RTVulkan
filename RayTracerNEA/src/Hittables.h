#pragma once

#include "Utilities.h"
#include <glm/glm.hpp>

#include "Ray.h"


struct hit_record
{
	glm::vec3 hit_point;
	glm::vec3 hit_normal;

	bool front_face;		// Is ray coming from inside or outside?
	float closest_t;		// Holds current closest t value

	glm::vec3 hit_albedo;	// Hit object intrinsic colour

	inline void set_face_normal(const Ray& ray, const glm::vec3& outward_normal) {
		front_face = glm::dot(ray.GetRayDirection(), outward_normal) < 0;
		hit_normal = front_face ? outward_normal : -outward_normal;
	}
};

class Hittables
{
public:
	virtual bool hit(const Ray& ray, float t_min, float t_max, hit_record& rec) const = 0;
};
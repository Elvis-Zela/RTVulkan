#include "Sphere.h"

#include "Ray.h"

/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------- Sphere Constructors -------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
Sphere::Sphere(Material material, glm::vec3 center, float r)
	: m_Center(center), m_Radius(r), mat(material)
{}

Sphere::~Sphere() {}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------- Check If Sphere Hit -------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
bool Sphere::hit(const Ray& ray, float t_min, float t_max, RayPayload& rec, rayType rt) const
{
	/* - solves vector equation of a circle to see if a ray has hit it - */
	glm::vec3 centerToOrigin = ray.GetRayOrigin() - m_Center;
	auto a = glm::dot(ray.GetRayDirection(), ray.GetRayDirection());
	auto halfB = glm::dot(centerToOrigin, ray.GetRayDirection());
	auto c = glm::dot(centerToOrigin, centerToOrigin) - ( m_Radius * m_Radius );

	/* - If discriminant is less than 0 no real solution - */
	auto discriminant = ( halfB * halfB ) - ( a * c );
	if (discriminant < 0) 
		return false;

	/* - calculate nearest root - */
	auto sqrtd = glm::sqrt(discriminant);
	auto root = (-halfB - sqrtd) / a;
	if (root < t_min || t_max < root)
	{
		root = (-halfB + sqrtd) / a;
		if (root < t_min || t_max < root)
			return false;
	}

	/* - Storing root - */
	rec.closestT = root;

	return true;
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

glm::vec3 Sphere::getNormal(const glm::vec3& hitPoint) const
{
	return (hitPoint - m_Center) / m_Radius;
}

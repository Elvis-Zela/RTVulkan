#include "Sphere.h"

#include "Ray.h"

/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------- Sphere Constructors -------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
Sphere::Sphere(glm::vec3 center, float r, glm::vec3 albedo)
	: m_Center(center), m_Radius(r), m_Albedo(albedo)
{}

Sphere::~Sphere() {}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------- Check If Sphere Hit -------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
bool Sphere::hit(const Ray& ray, float t_min, float t_max, RayPayload& rec) const
{
	/* - solves vector equation of a circle to see if a ray has hit it - */
	glm::vec3 originToCenter = ray.GetRayOrigin() - m_Center;
	auto a = glm::dot(ray.GetRayDirection(), ray.GetRayDirection());
	auto halfB = glm::dot(originToCenter, ray.GetRayDirection());
	auto c = glm::dot(originToCenter, originToCenter) - ( m_Radius * m_Radius );

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

/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------ Add To Payload If Hit ------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void Sphere::ClosestHitShader(const Ray& ray, RayPayload& rec) const
{
	/* - Point of intersection - */
	rec.hitPoint = ray.At(rec.closestT);

	/* - Storing the normal at that point, pointing out of the sphere - */
	glm::vec3 outwardNormal = (rec.hitPoint - m_Center) / m_Radius;
	/* - Storing the normal in the scene direction - */
	bool faceNoraml = glm::dot(outwardNormal, ray.GetRayDirection()) < 0;
	rec.hitNormal = faceNoraml ? outwardNormal : -outwardNormal;

	rec.hitAlbedo = m_Albedo;
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

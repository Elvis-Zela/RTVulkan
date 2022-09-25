#include "Sphere.h"

#include "Ray.h"

Sphere::Sphere(glm::vec3 center, float r, glm::vec3 albedo)
	: m_Center(center), m_Radius(r), m_Albedo(albedo)
{}

Sphere::~Sphere() {}

//solves vector equation of a circle to see if a ray has hit it
bool Sphere::hit(const Ray& ray, float t_min, float t_max, hit_record& rec) const
{
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
	rec.closest_t = root;
	/* - Point of intersection - */
	rec.hit_point = ray.At(rec.closest_t);
	/* - Storing the normal at that point, pointing out of the sphere - */
	glm::vec3 outwardNormal = (rec.hit_point - m_Center) / m_Radius;
	rec.set_face_normal(ray, outwardNormal);
	rec.hit_albedo = m_Albedo;

	return true;
}


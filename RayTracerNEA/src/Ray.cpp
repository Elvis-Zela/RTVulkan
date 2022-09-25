#include "Ray.h"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
	: m_Origin(origin), m_Direction(direction)
{}

void Ray::SetRayOrigin(glm::vec3 origin)
{
	m_Origin = origin;
}

void Ray::SetRayDirection(glm::vec3 direction)
{
	m_Direction = direction;
}

glm::vec3 Ray::At(float t) const
{
	return m_Origin + m_Direction * t;
}
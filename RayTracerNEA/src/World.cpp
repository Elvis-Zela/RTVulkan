#include "World.h"
#include "Ray.h"
#include "Materials.h"
#include "Sphere.h"

/* ------------------------------------------------------------------------------------------------ */
/* -------------------------------------- World Constructor --------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
World::World(std::shared_ptr<Sphere> object)
{
	m_SceneGeometry.push_back(object);
}

World::~World()
{
	/* - Clears World - */
	m_SceneGeometry.clear(); 
	m_Lights.clear();
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------ Add Objects To World -------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void World::AddObject(std::shared_ptr<Sphere> object)
{
	m_SceneGeometry.push_back(object);
}

void World::AddLight(std::shared_ptr<Lights> light)
{
	m_Lights.push_back(light);
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------ Check If World Is Hit ------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
bool World::hit(const Ray& ray, float t_min, float t_max, RayPayload& rec, rayType rt) const
{
	rec.object = nullptr;
	auto closestSoFar = t_max;

	for (int i = 0; i < m_SceneGeometry.size(); i ++)
	{
		Sphere* object = m_SceneGeometry[i].get();

		if (m_SceneGeometry[i]->hit(ray, t_min, closestSoFar, rec, rt))
		{
			if (rt == kShadowRay && object->mat.type == kDielectric) continue;
			rec.object = m_SceneGeometry[i].get();
			closestSoFar = rec.closestT;
		}
	}

	return (rec.object != nullptr);
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

#include "World.h"
#include "Ray.h"

World::~World()
{
	/* - Clears World - */
	m_Scene.clear(); 
}

bool World::hit(const Ray& ray, float t_min, float t_max, hit_record& rec) const
{
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;

	for (const auto& object : m_Scene)
	{
		if (object->hit(ray, t_min, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.closest_t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}
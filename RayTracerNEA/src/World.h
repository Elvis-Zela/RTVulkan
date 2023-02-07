#pragma once
#include "Utilities.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "Materials.h"
#include "Ray.h"
#include "Lights.h"

class World
{
public:
	/* - Constructors - */
	World() {}
	World(std::shared_ptr<Sphere> object);
	~World();

	/* - Adds a new object to the scene - */
	void AddObject(std::shared_ptr<Sphere> object);
	void AddLight(std::shared_ptr<Lights> light);

	size_t SceneSize() const { return m_SceneGeometry.size(); }

public:
	virtual bool hit(const Ray& ray, float t_min, float t_max, RayPayload& rec, rayType rt = kPrimaryRay) const;

public:
	std::vector<std::shared_ptr<Sphere>> m_SceneGeometry;
	std::vector<std::shared_ptr<Lights>> m_Lights;
};

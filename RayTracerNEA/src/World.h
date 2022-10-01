#pragma once
#include "Utilities.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "Hittables.h"

using std::shared_ptr;
using std::vector;

class Ray;

class World : public Hittables
{
public:
	/* - Constructors - */
	World() {}
	World(shared_ptr<Hittables> object);
	~World();

	/* - Adds a new object to the scene - */
	void Add(shared_ptr<Hittables> object);

	size_t SceneSize() const { return m_Scene.size(); }

	virtual bool hit(const Ray& ray, float t_min, float t_max, RayPayload& rec) const override;
	virtual void ClosestHitShader(const Ray& ray, RayPayload& rec) const override;

private:
	std::vector<shared_ptr<Hittables>> m_Scene;
};

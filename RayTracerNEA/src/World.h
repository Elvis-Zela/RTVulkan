#pragma once
#include "Utilities.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "Hittables.h"

using std::shared_ptr;
using std::vector;

class Ray;
class Material;

class World : public Hittables
{
public:
	/* - Constructors - */
	World() {}
	World(shared_ptr<Hittables> object);
	~World();

	/* - Adds a new object to the scene - */
	int AddObject(shared_ptr<Hittables> object);
	int AddMaterial(shared_ptr<Material> mat);
	shared_ptr<Material> GetMat(int matIndex) const { return m_ObjectMaterials[matIndex]; }
	shared_ptr<Hittables> GetObj(int objIndex) const { return m_SceneGeometry[objIndex]; }

	size_t SceneSize() const { return m_SceneGeometry.size(); }

public:
	virtual bool hit(const Ray& ray, float t_min, float t_max, RayPayload& rec) const override;
	virtual void ClosestHitShader(const Ray& ray, RayPayload& rec) const override;
	virtual int GetMatIndex() const override { return -1; }

public:
	std::vector<shared_ptr<Hittables>> m_SceneGeometry;
	std::vector<shared_ptr<Material>> m_ObjectMaterials;
};

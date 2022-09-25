#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

#include "Utilities.h"

class Camera;
class Hittables;
class Ray;

class Renderer
{
public:
/* - Constructor - */
	Renderer() = default;

	void Render(const Hittables& world, const Camera& camera);
	void IfResizing(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

/* - Private Methods - */
private:
	glm::vec4 TraceRay(const Ray& ray, const Hittables& world);

/* - Attributes - */
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageDataBuffer = nullptr;
};

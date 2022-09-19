#pragma once

#include "Walnut/Image.h"

#include "Camera.h"
#include "Ray.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
/* - Constructor - */
	Renderer() = default;

	void Render(const Camera& camera);
	void Resizing(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

/* - Private Methods - */
private:
	glm::vec4 TraceRay(const Ray& ray);

/* - Attributes - */
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageDataBuffer = nullptr;
};
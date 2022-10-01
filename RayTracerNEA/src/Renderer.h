#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

#include "Utilities.h"

class Camera;
class Hittables;
struct RayPayload;
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
	glm::vec4  RayGeneration(uint32_t x, uint32_t y);
	void MissShader(const Ray& ray, RayPayload& payload);
	void TraceRay(const Ray& ray, RayPayload& payload);

	/* - Public Attributes for User Defined settings - */
public:
	int bounceDepth = 1;

/* - Attributes - */
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageDataBuffer = nullptr;

	/* - Pointers to the camera and world so that all methods can access them*/
	const Camera*	  m_Camera = nullptr;
	const Hittables*  m_World  = nullptr;
};

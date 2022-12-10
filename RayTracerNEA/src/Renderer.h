#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

#include "Utilities.h"
#include "Materials.h"

class Camera;
class World;
struct RayPayload;
class Ray;

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
		int bounceDepth = 1;
	};
public:
/* - Constructor - */
	Renderer() = default;

	void Render(const World& world, const Camera& camera);
	void IfResizing(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }

/* - Private Methods - */
private:
	glm::vec4  RayGeneration(uint32_t x, uint32_t y);
	void MissShader(const Ray& ray, RayPayload& payload);
	void TraceRay(const Ray& ray, RayPayload& payload);

/* - Attributes - */
private:
	Settings m_Settings;

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageDataBuffer = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;

	/* - Pointers to the camera and world so that all methods can access them - */
	const Camera*	  m_Camera = nullptr;
	const World*	  m_World  = nullptr;
};

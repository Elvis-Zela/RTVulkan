#include "Renderer.h"

#include "Walnut/Random.h"

#include "Camera.h"
#include "Ray.h"
#include "Hittables.h"

void Renderer::IfResizing(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		/* - If dimensions are unchanged no resizing is necessary - */
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		/* - Releases and Reallocates GPU memory within same object - */
		m_FinalImage->Resize(width, height);

	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageDataBuffer;
	m_ImageDataBuffer = new uint32_t[width * height];
}

void Renderer::Render(const Hittables& world, const Camera& camera)
{
	Ray ray;
	ray.SetRayOrigin(camera.GetCameraPosition());

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			ray.SetRayDirection(camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()]);
			glm::vec4 color = TraceRay(ray, world);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageDataBuffer[x + y * m_FinalImage->GetWidth()] = Utilities::ToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageDataBuffer);
}

glm::vec4 Renderer::TraceRay(const Ray& ray, const Hittables& world)
{
	hit_record rec;

	if (!world.hit(ray, NEAR_EPSILON, FLOAT_LARGE, rec))
		return glm::vec4(BASE_SKY_COLOUR, 1.0f);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
	float lightIntensity = glm::max(glm::dot(rec.hit_normal, -lightDir), 0.0f); // == cos(angle)

	glm::vec3 objectColor = rec.hit_albedo;
	objectColor *= lightIntensity;
	return glm::vec4(objectColor, 1.0f);
}

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

/* - Uses multiple rays per sample to average out the colour of a pixel, results in smoother shapes (anti-aliasing), but blurrier - */
/* - Currently very ineficient - */

void Renderer::SampleRender(const Hittables& world, const Camera& camera, const int sampleSize)
{
	Ray ray;
	ray.SetRayOrigin(camera.GetCameraPosition());

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color(0.0f, 0.0f, 0.0f, 1.0f);

			for (int s = 0; s < sampleSize; ++s)
			{
				for (int s2 = 0; s2 < sampleSize; ++s2)
				{
					ray.SetRayDirection(camera.GetRayDirections()[(x + s) + (y + s2) * m_FinalImage->GetWidth()]);
					/* - Add ray colour to a total to get a sum of colour around a pixel - */
					color += TraceRay(ray, world);
				}
			}

			/* - dividing colour by amount of samples to get appropriate colour - */

			float scale = 1.0f / (float)(sampleSize * sampleSize);
			color *= scale;
			color.a = 1.0f;
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			m_ImageDataBuffer[x + y * m_FinalImage->GetWidth()] = Utilities::ToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageDataBuffer);
}

#include "Renderer.h"

#include "Walnut/Random.h"

#include <memory>

#include "Camera.h"
#include "Ray.h"
#include "Hittables.h"

/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------- Resizing Option ------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
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
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* ----------------------------------- Main Render Function --------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void Renderer::Render(const Hittables& world, const Camera& camera)
{
	m_Camera = &camera;
	m_World  = &world;

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color = RayGeneration(x, y);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageDataBuffer[x + y * m_FinalImage->GetWidth()] = Utilities::ToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageDataBuffer);
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* ----------------------------------- Ray Generation Shader -------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
glm::vec4 Renderer::RayGeneration(uint32_t x, uint32_t y)
{
	/* - declaring varaibles - */
	Ray ray;
	RayPayload payload;
	glm::vec3 colour(0.0f);

	/* - Ray should originate from the camera - */
	ray.SetRayOrigin(m_Camera->GetCameraPosition());
	ray.SetRayDirection(m_Camera->GetRayDirections()[x + y * m_FinalImage->GetWidth()]);

	/* - Temporary mesure to say how much the light from / colour from a bounce affects object colour - */
	float colourContribution = 1.0f;
	for (int i = 0; i < bounceDepth; i++)
	{
		TraceRay(ray, payload);
		if (payload.closestT < 0.0f)
		{
			colour += BASE_SKY_COLOUR * colourContribution;
			break;
		}

		/* - Retrieving the hit object's albedo*/
		glm::vec3 objectColor = payload.hitAlbedo;

		/* - simple lighting hack to get shading for the moment - */
		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
		/* - Sets a light direction and depending on the angle made by the hit normal the direction we decide how lit it is - */
		float lightIntensity = glm::max(glm::dot(payload.hitNormal, -lightDir), 0.0f); // == cos(angle)
		objectColor *= lightIntensity;

		payload.hitAlbedo = objectColor;

		colour = payload.hitAlbedo * colourContribution;
		/* - Decreasing colour contribution for next bounces - */
		colourContribution *= 0.75f;

		ray.SetRayOrigin(payload.hitPoint + payload.hitNormal * NEAR_EPSILON);
		ray.SetRayDirection(glm::reflect(ray.GetRayDirection(), payload.hitNormal));
	}

	return glm::vec4(colour, 1.0f);
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* --------------------------------- Tracing Ray Into The Scene ----------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void Renderer::TraceRay(const Ray& ray, RayPayload& payload)
{
	/* - If the world hit function doesn't return a value then no geometry was hit, call miss shader - */
	if (!m_World->hit(ray, NEAR_EPSILON, FLOAT_LARGE, payload))
	{
		MissShader(ray, payload);
		return;
	}

	m_World->ClosestHitShader(ray, payload);
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------ */
/* ---------------------------------------- Miss Shader ------------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void Renderer::MissShader(const Ray& ray, RayPayload& payload)
{
	payload.closestT = -1.0f;
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

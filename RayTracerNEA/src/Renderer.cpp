#include "Renderer.h"

#include <vector>
#include <memory>

#include "Camera.h"
#include "Ray.h"
#include "World.h"

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

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* ----------------------------------- Main Render Function --------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void Renderer::Render(const World& world, const Camera& camera)
{
	/* - Allows use of the camera and world by all renderer functions without passing them - */
	m_Camera = &camera;
	m_World  = &world;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

	/* - Rendering from top to bottom, lef to right - */
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 colour = RayGeneration(x, y);
			m_AccumulationData[x + y * m_FinalImage->GetWidth()] += colour;

			colour = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
			colour /= (float)m_FrameIndex;

			colour = glm::clamp(colour, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageDataBuffer[x + y * m_FinalImage->GetWidth()] = Utilities::ToRGBA(colour);
		}
	}

	m_FinalImage->SetData(m_ImageDataBuffer);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
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
	for (int i = 0; i < GetSettings().bounceDepth; i++)
	{
		/* - Casts ray to the scene - */
		TraceRay(ray, payload);
		/* - If colosest T value is set to a negative value, miss shader was used so all geometry missed - */
		if (payload.closestT < 0.0f)
		{
			/* - Setting miss colour to the background colour x its contribution - */
			colour += BASE_SKY_COLOUR * colourContribution;
			break;
		}

		glm::vec3 attenuation;

		std::shared_ptr<Hittables> obj = m_World->GetObj(payload.objIdx);
		std::shared_ptr<Material> mat = m_World->GetMat(obj->GetMatIndex());

		if (!mat->Scatter(ray, payload, attenuation, ray))
			break;

		/* - Hardcoding in a simple light source - */
		glm::vec3 lightPos{ 10.0f, 25.0f, 15.0f };
		glm::vec3 lightDir = glm::normalize(payload.hitPoint - lightPos);

		/* - Sets a light direction and depending on the angle made by the hit normal the direction we decide how lit it is - */
		float lightIntensity = glm::max(glm::dot(payload.hitNormal, -lightDir), 0.0f); // == cos(angle)
		attenuation *= lightIntensity; 
		colour += attenuation * colourContribution;

		colourContribution *= 0.35f;

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
	/* - calling the object's closest hit shader - */
	m_World->m_SceneGeometry[payload.objIdx]->ClosestHitShader(ray, payload);
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* ---------------------------------------- Miss Shader ------------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void Renderer::MissShader(const Ray& ray, RayPayload& payload)
{
	/* - Setting closest T value found as negative to easily check if ray missed - */
	payload.closestT = -1.0f;
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

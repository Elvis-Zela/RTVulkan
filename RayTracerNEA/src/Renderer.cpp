#include "Renderer.h"

#include <vector>
#include <memory>
#include <execution>

#include "Camera.h"
#include "Ray.h"
#include "World.h"
#include "Lights.h"
#include "Sphere.h"

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

	m_ImgHorizontalIter.resize(width);
	m_ImgVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImgHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImgVerticalIter[i] = i;
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
#define MT 1
#if (MT == 1)

	std::for_each(std::execution::par, m_ImgVerticalIter.begin(), m_ImgVerticalIter.end(),
		[this](uint32_t y)
		{
			std::for_each(std::execution::par, m_ImgHorizontalIter.begin(), m_ImgHorizontalIter.end(),
			[this, y](uint32_t x)
				{
					glm::vec4 colour = RayGeneration(x, y);
					m_AccumulationData[x + y * m_FinalImage->GetWidth()] += colour;

					colour = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
					colour /= (float)m_FrameIndex;

					colour = glm::clamp(colour, glm::vec4(0.0f), glm::vec4(1.0f));
					m_ImageDataBuffer[x + y * m_FinalImage->GetWidth()] = Utilities::ToRGBA(colour);
				});
		});

#else

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

#endif

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
	glm::vec3 finalColour(0.0f);

	/* - Ray should originate from the camera - */
	ray.SetRayOrigin(m_Camera->GetCameraPosition());
	ray.SetRayDirection(m_Camera->GetRayDirections()[x + y * m_FinalImage->GetWidth()]);

	finalColour = ComputeColour(ray, payload, 1);

	return glm::vec4(finalColour, 1.0f);
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------- Computing Colour ----------------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
glm::vec3 Renderer::ComputeColour(Ray& ray, RayPayload& payload, int depth)
{
	if (depth > m_Settings.maxBounceDepth) { return BASE_SKY_COLOUR; }
	
	/* - Temporary mesure to say how much the light from / colour from a bounce affects object colour - */
	float colourContribution = 1.0f;
	glm::vec3 colour(0.0f);

	for (int i = depth; i <= m_Settings.maxBounceDepth; i++)
	{
		/* - Casts ray to the scene - */
		glm::vec3 hitNormal;
		TraceRay(ray, payload, hitNormal);
		/* - If colosest T value is set to a negative value, miss shader was used so all geometry missed - */
		if (payload.closestT < 0.0f)
		{
			/* - Setting miss colour to the background colour x its contribution - */
			colour += BASE_SKY_COLOUR * colourContribution;
			break;
		}

		Sphere* obj = payload.object;
		Material mat = obj->mat;
		glm::vec3 hitPoint = ray.At(payload.closestT);
		glm::vec3 normal = (glm::dot(hitNormal, ray.GetRayDirection()) < 0) ? hitNormal : -hitNormal;

		glm::vec3 rayDir = ray.GetRayDirection();
		glm::vec3 attenuation(0.0f);
		bool Scatter;

		switch (mat.type)
		{
		case kLambertian:
		{
			glm::vec3 scatterDirection = hitNormal + Walnut::Random::InUnitSphere();

			if (Utilities::nearZero(scatterDirection))
				scatterDirection = hitNormal;

			ray = Ray(hitPoint + normal * NEAR_EPSILON, scatterDirection);
			attenuation = getLighting(mat.Albedo, hitPoint, normal);
			colour += attenuation * colourContribution;
		}
		Scatter = true;

		break;

		case kMetal:
		{
			glm::vec3 reflected = glm::reflect(glm::normalize(ray.GetRayDirection()), hitNormal);
			ray = Ray(hitPoint, reflected + mat.Fuzz * Walnut::Random::InUnitSphere());
			attenuation = getLighting(mat.Albedo, hitPoint, normal);
			colour += attenuation * colourContribution;
		}
		Scatter = (glm::dot(ray.GetRayDirection(), hitNormal) > 0);

		break;

		case kReflective:
		{
			bool outside = glm::dot(rayDir, hitNormal) < 0;
			glm::vec3 bias = 0.0001f * hitNormal;
			glm::vec3 reflectionDirection = glm::normalize(reflect(rayDir, hitNormal));
			glm::vec3 reflectionOrigin = outside ? hitPoint + bias : hitPoint - bias;
			Ray reflectionRay = Ray(reflectionOrigin, reflectionDirection);
			glm::vec3 reflectionColour = ComputeColour(reflectionRay, payload, depth + 1);
			attenuation += reflectionColour;
			colour += attenuation * colourContribution;
		}
		Scatter = true;

		break;

		case kDielectric:
		{
			glm::vec3 refractionColour = glm::vec3(0), reflectionColour = glm::vec3(0);
			float kr = MatUtils::fresnel(rayDir, hitNormal, mat.IndexOfRefraction);
			bool outside = glm::dot(rayDir, hitNormal) < 0;
			glm::vec3 bias = NEAR_EPSILON * hitNormal;

			if (kr < 1.0f)
			{
				glm::vec3 refractionDirection = MatUtils::refract(rayDir, hitNormal, mat.IndexOfRefraction);
				glm::vec3 refractionOrigin = outside ? hitPoint - bias : hitPoint + bias;
				Ray refractionRay = Ray(refractionOrigin, refractionDirection);
				refractionColour = ComputeColour(refractionRay, payload, depth + 1);
			}

			glm::vec3 reflectionDirection = MatUtils::reflect(rayDir, hitNormal);
			glm::vec3 reflectionOrigin = outside ? hitPoint + bias : hitPoint - bias;
			Ray reflectionRay = Ray(reflectionOrigin, reflectionDirection);
			reflectionColour = ComputeColour(reflectionRay, payload, depth + 1);
			attenuation += reflectionColour * kr + refractionColour * (1 - kr);
			colour += attenuation * colourContribution;
		}
			Scatter = true;
			break;

		default:
			Scatter = false;
			break;
		}

		if (!Scatter) break;

		colourContribution *= 0.3f;

	}

	return colour;
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------ */
/* --------------------------------- Tracing Ray Into The Scene ----------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
void Renderer::TraceRay(const Ray& ray, RayPayload& payload, glm::vec3& hitNormal)
{
	/* - If the world hit function doesn't return a value then no geometry was hit, call miss shader - */
	if (!m_World->hit(ray, NEAR_EPSILON, FLOAT_LARGE, payload))
	{
		payload.closestT = -1.0f;
		return;
	}

	hitNormal = payload.object->getNormal(ray.At(payload.closestT));
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------ */
/* --------------------------------- Computing Lighting Details ----------------------------------- */
/* ------------------------------------------------------------------------------------------------ */
glm::vec3 Renderer::getLighting(const glm::vec3& albedo, const glm::vec3& hitPoint, const glm::vec3& hitNormal)
{
	glm::vec3 lightDir(0.0f);
	glm::vec3 lightInt(0.0f);
	glm::vec3 shadedColour(0.0f);
	float dist;
	RayPayload pay;

	for (const auto& light : m_World->m_Lights)
	{
		light->getShadingDetails(hitPoint, lightDir, lightInt, dist);
		Ray ray = Ray(hitPoint + hitNormal * NEAR_EPSILON, -lightDir);

		bool vis = !m_World->hit(ray, NEAR_EPSILON, dist, pay, kShadowRay);
		shadedColour += (float)vis * albedo * lightInt * std::max(0.0f, glm::dot(hitNormal, -lightDir));
	}
	return shadedColour;
}
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

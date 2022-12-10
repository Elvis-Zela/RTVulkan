#pragma once

#include <glm/glm.hpp>
#include <vector>

class Ray;
struct RayPayload;

class Material
{
public:
	virtual bool Scatter(const Ray& rayIn, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const = 0;
	virtual glm::vec3 Emitted() const {
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	virtual glm::vec3* GetAlbedo() = 0;
};

class RoughMat : public Material
{
public:
	RoughMat(glm::vec3 alb, float rough) : m_Albedo(alb), m_Roughness(rough) {}

public:
	virtual bool Scatter(const Ray& rayIn, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const override;
	virtual glm::vec3* GetAlbedo() override	{ return &m_Albedo; }

public:
	glm::vec3 m_Albedo;
	float	  m_Roughness;
};

class Metal : public Material
{
public:
	Metal(const glm::vec3 alb, float f) : m_Albedo(alb), m_Fuzz(f < 1 ? f : 1) {}

public:
	virtual bool Scatter(const Ray& rayIn, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const override;
	virtual glm::vec3* GetAlbedo() override { return &m_Albedo; }

public:
	glm::vec3 m_Albedo;
	float	  m_Fuzz;
};

class Lambertian : public Material
{
public:
	Lambertian(const glm::vec3& alb) : m_Albedo(alb) {}

public:
	virtual bool Scatter(const Ray& rayIn, const RayPayload& payload, glm::vec3& attenuation, Ray& scattered) const override;
	virtual glm::vec3* GetAlbedo() override { return &m_Albedo; }

public:
	glm::vec3 m_Albedo;
};

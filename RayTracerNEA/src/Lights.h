#pragma once
#include <glm/gtc/type_ptr.hpp>

enum LightType { kPoint, kDistant };

class Lights
{
public:
	virtual void getShadingDetails(const glm::vec3& hitPos, glm::vec3& lightDir, glm::vec3& lightInt, float& dist) const = 0;
	Lights(glm::vec3 lColour, float lInt = 1.0f);

public:
	glm::vec3 colour;
	float	  intensity;
	LightType type;

	glm::vec3 pos;
	glm::vec3 direction;
};

class PointLight : public Lights
{
public:
	void getShadingDetails(const glm::vec3& hitPos, glm::vec3& lightDir, glm::vec3& lightInt, float& dist) const override;
	PointLight(glm::vec3 lPos, glm::vec3 lColour, float lInt = 1.0f);
};

class DistantLight : public Lights
{
public:
	void getShadingDetails(const glm::vec3& hitPos, glm::vec3& lightDir, glm::vec3& lightInt, float& dist) const override;
	DistantLight(glm::vec3 lDir, glm::vec3 lColour, float lInt = 1.0f);
};
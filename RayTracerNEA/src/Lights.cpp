#include "Lights.h"
#include "Utilities.h"

Lights::Lights(glm::vec3 lColour, float lInt)
{
    colour = lColour;
    intensity = lInt;
}

PointLight::PointLight(glm::vec3 lPos, glm::vec3 lColour, float lInt) : Lights(lColour, lInt)
{
    pos = lPos;
    type = kPoint;
}

DistantLight::DistantLight(glm::vec3 lDir, glm::vec3 lColour, float lInt) : Lights(lColour, lInt)
{
    direction = glm::normalize(lDir);
    type = kDistant;
}

void PointLight::getShadingDetails(const glm::vec3& hitPos, glm::vec3& lightDir, glm::vec3& lightInt, float& dist) const
{
    lightDir = hitPos - pos;
    float r2 = glm::dot(lightDir, lightDir);
    dist = glm::sqrt(r2);
    lightDir /= dist;
    lightInt = intensity * colour / (float)(4 * PI * r2);
}

void DistantLight::getShadingDetails(const glm::vec3& hitPos, glm::vec3& lightDir, glm::vec3& lightInt, float& dist) const
{
    lightDir = direction;
    lightInt = intensity * colour;
    dist = FLOAT_LARGE;
}

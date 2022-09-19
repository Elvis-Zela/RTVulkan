#pragma once

#include <glm/glm.hpp>
#include <vector>

class Camera
{
public:
	Camera(float vFOV, float near, float far);

	void Update(float ts);
	void Resizing(uint32_t width, uint32_t height);

	const glm::mat4& GetProjectionM() const { return m_ProjectionM; }
	const glm::mat4& GetInverseProjectionM() const { return m_InverseProjectionM; }
	const glm::mat4& GetViewM() const { return m_ViewM; }
	const glm::mat4& GetInverseViewM() const { return m_InverseViewM; }

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetDirection() const { return m_Direction; }

	const std::vector<glm::vec3>& GetRayDirections() const { return m_RayDirections; }

	float GetRotationSpeed();

private:
	void RecalculateProjectionM();
	void RecalculateViewM();
	void RecalculateRayDirections();

private:
	glm::mat4 m_ProjectionM{ 1.0f };
	glm::mat4 m_ViewM{ 1.0f };
	glm::mat4 m_InverseProjectionM{ 1.0f };
	glm::mat4 m_InverseViewM{ 1.0f };

	float m_vFOV	= 45.0f;
	float m_Near	= 0.0001f;
	float m_Far		= 100.0f;

	glm::vec3 m_Position{ 0.0f,0.0f,0.0f };
	glm::vec3 m_Direction{ 0.0f, 0.0f, 0.0f };

	/* - Cached Ray Directions - */
	std::vector<glm::vec3> m_RayDirections;

	glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};
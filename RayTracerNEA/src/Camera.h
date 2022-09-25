#pragma once

#include <vector> 

#include "Utilities.h"

#define DEFAULT_FORWARD_DIRECTION	glm::vec3{ 0.0f, 0.0f, -1.0f }
#define DEFAULT_RIGHT_DIRECTION		glm::vec3{ 1.0f, 0.0f, 0.0f }
#define WORLD_UP_DIRECTION		glm::vec3{ 0.0f,1.0f ,0.0f }

#define CAMERA_MOVEMENT_SPEED		5.0f
#define MOUSE_SENSITIVITY		0.2f

class Camera
{
public:
	Camera();
	Camera(glm::vec3 pos, glm::vec3 lookat);
	~Camera();

	void Update(float ts);
	void IfResizing(uint32_t width, uint32_t height);

	/* - Get View and Projection Matrices - */
	const glm::mat4& GetProjectionM() const			{ return m_ProjectionM; }
	const glm::mat4& GetViewM() const			{ return m_ViewM; }
	/* - Get Their Inverses - */
	const glm::mat4& GetInverseProjectionM() const		{ return m_InverseProjectionM; }
	const glm::mat4& GetInverseViewM() const		{ return m_InverseViewM; }

	const glm::vec3& GetCameraPosition() const		{ return m_CameraPosition; }
	const glm::vec3& GetCameraForwardDirection() const	{ return m_ForwardDirection; }
	const glm::vec3& GetCameraRightDirection() const	{ return m_RightDirection; }

	const std::vector<glm::vec3>& GetRayDirections() const	{ return m_RayDirections; }

public:
	/* - User Defined Settings - */
	/* - Initialised to default values - */
	float MouseSensitivity = MOUSE_SENSITIVITY;
	float CameraFlySpeed = CAMERA_MOVEMENT_SPEED;

private:
	void RecalculateProjectionM();
	void RecalculateViewM();
	void RecalculateRayDirections();

private:
	float m_vFOV	= 45.0f;
	float m_Near	= NEAR_EPSILON;
	float m_Far	= FLOAT_LARGE;

	glm::mat4 m_ProjectionM{ 1.0f };
	glm::mat4 m_ViewM{ 1.0f };
	glm::mat4 m_InverseProjectionM{ 1.0f };
	glm::mat4 m_InverseViewM{ 1.0f };

	glm::vec3 m_CameraPosition{ 0.0f };

	glm::vec3 m_UpDirection		= WORLD_UP_DIRECTION;
	glm::vec3 m_ForwardDirection	= DEFAULT_FORWARD_DIRECTION;
	glm::vec3 m_RightDirection	= DEFAULT_RIGHT_DIRECTION;

	/* - Cached Ray Directions - */
	std::vector<glm::vec3> m_RayDirections;
	/* - Chached previous mouse pos - */
	glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };
	/* - Cached width and height of the viewport - */
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};

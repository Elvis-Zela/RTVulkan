#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

#define CAMERA_ROTATION_SPEED		0.3f

using namespace Walnut;

/* - Camera Constructor - */
Camera::Camera() 
{
	m_CameraPosition = { 0.0f, 0.5f, 8.0f };
}

Camera::Camera(glm::vec3 pos, glm::vec3 lookat)
{
	m_CameraPosition = pos;
	m_ForwardDirection = lookat;
}

Camera::~Camera()
{}

/* - In case viewport window has been resized - */
/* - Also used at start up to initialise attributes - */
void Camera::IfResizing(uint32_t width, uint32_t height)
{
	if (width == m_ViewportWidth && height == m_ViewportHeight)
		return;

	m_ViewportWidth = width;
	m_ViewportHeight = height;

	RecalculateProjectionM();
	RecalculateRayDirections();
}

void Camera::RecalculateProjectionM()
{
	m_ProjectionM = glm::perspectiveFov(glm::radians(m_vFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_Near, m_Far);
	m_InverseProjectionM = glm::inverse(m_ProjectionM);
}

void Camera::RecalculateViewM()
{
	/* - Camera Origin, Point Position, Up vector - */
	m_ViewM = glm::lookAt(m_CameraPosition, m_CameraPosition + m_ForwardDirection, WORLD_UP_DIRECTION);
	m_InverseViewM = glm::inverse(m_ViewM);
}

void Camera::RecalculateRayDirections()
{
	/* - Allocates enough space in the vector for all rays in the iamge - */
	m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

	for (uint32_t y = 0; y < m_ViewportHeight; y++)
	{
		for (uint32_t x = 0; x < m_ViewportWidth; x++)
		{
			/* - uv coordinates mapped between: 0 -> 1 - */
			glm::vec2 coord = { (float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight };
			/* - mapped between: -1 -> 1 - */
			coord = coord * 2.0f - 1.0f; 

			/* - Projective Space coordinates (x, y, z, w) - */
			glm::vec4 target = m_InverseProjectionM * glm::vec4(coord.x, coord.y, 1, 1);
			/* - mapping from projective space to world coordinates (x, y, z) - */
			glm::vec3 rayDirection;

			if(target.w == 0)
				rayDirection = glm::vec3(m_InverseViewM * glm::normalize(target));
			else
				rayDirection = glm::vec3(m_InverseViewM * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));

			m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
		}
	}
}

void Camera::Update(float ts)
{
	glm::vec2 mousePos = Input::GetMousePosition();
	/* - amount that the mouse has moved from previous pos - */
	glm::vec2 displacement = (mousePos - m_LastMousePosition) * MouseSensitivity * 0.01f; // sens dampening
	m_LastMousePosition = mousePos;


	if (!Input::IsMouseButtonDown(MouseButton::Right))
	{
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	/* - Locks Cursor to the boudaries of the window and hides it, only if RMB is down - */
	Input::SetCursorMode(CursorMode::Locked);

	bool moved = false;
	m_RightDirection = glm::cross(m_ForwardDirection, m_UpDirection);

	/* - Forward and Back movement - */
	if (Input::IsKeyDown(KeyCode::W))
	{
		/* - Incrementing current Pos with camera direction and constants*/
		m_CameraPosition += m_ForwardDirection * CameraFlySpeed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::S))
	{
			m_CameraPosition -= m_ForwardDirection * CameraFlySpeed * ts;
			moved = true;
	}
	/* - Left and Right Movement - */
	if (Input::IsKeyDown(KeyCode::A))
	{
		m_CameraPosition -= m_RightDirection * CameraFlySpeed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::D))
	{
		m_CameraPosition += m_RightDirection * CameraFlySpeed * ts;
		moved = true;
	}
	/* - Up and Down Movement - */
	if (Input::IsKeyDown(KeyCode::Space))
	{
		m_CameraPosition += m_UpDirection * CameraFlySpeed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::LeftShift))
	{
		m_CameraPosition -= m_UpDirection * CameraFlySpeed * ts;
		moved = true;
	}

	/* - Camera rotations with mouse - */
	if (displacement.x != 0.0f || displacement.y != 0.0f)
	{
		float pitch 	= displacement.y * CAMERA_ROTATION_SPEED;
		float yaw	= displacement.x * CAMERA_ROTATION_SPEED;

		/* - rotation quaternion - */
		/* - normalised quaternion with combined rotation, pitch and yaw - */
		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitch, m_RightDirection),
			glm::angleAxis(-yaw, WORLD_UP_DIRECTION)));
		/* - new forward direction = old direction * quaternion - */
		m_ForwardDirection = glm::rotate(q, m_ForwardDirection);	

		moved = true;
	}

	/* - caching old values if not moved - */

	if (moved)
	{
		RecalculateViewM();
		RecalculateRayDirections();
	}
}

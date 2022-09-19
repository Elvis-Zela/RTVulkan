#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

using namespace Walnut;

/* - Camera Constructor - */
Camera::Camera(float vFOV, float near, float far)
	: m_vFOV(vFOV), m_Near(near), m_Far(far)
{
	m_Direction = glm::vec3(0, 0, -1);
	m_Position = glm::vec3(0, 0, 3);
}

/* - In case viewport window has been resized - */
void Camera::Resizing(uint32_t width, uint32_t height)
{
	if (width == m_ViewportWidth && height == m_ViewportHeight)
		return;

	m_ViewportWidth = width;
	m_ViewportHeight = height;

	RecalculateProjectionM();
	RecalculateRayDirections();
}

/* - Allow for user defined sensitivity - */
float Camera::GetRotationSpeed()
{
	return 0.3f;
}

void Camera::RecalculateProjectionM()
{
	m_ProjectionM = glm::perspectiveFov(glm::radians(m_vFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_Near, m_Far);
	m_InverseProjectionM = glm::inverse(m_ProjectionM);
}

void Camera::RecalculateViewM()
{
	/* - Camera Origin, Point Position, Up vector - */
	m_ViewM = glm::lookAt(m_Position, m_Position + m_Direction, glm::vec3(0, 1, 1));
	m_InverseViewM = glm::inverse(m_ViewM);
}

void Camera::RecalculateRayDirections()
{
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
			glm::vec3 rayDirection = glm::vec3(m_InverseViewM * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
			m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
		}
	}
}

void Camera::Update(float ts)
{
	glm::vec2 mousePos = Input::GetMousePosition();
	/* - amount that the mouse has moved from previous pos - */
	glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
	m_LastMousePosition = mousePos;

	if (!Input::IsMouseButtonDown(MouseButton::Right))
	{
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	/* - Locks Cursor to the boudaries of the window and hides it, only if RMB is down - */
	Input::SetCursorMode(CursorMode::Locked);

	bool moved = false;

	constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
	/* - Camera vector pointing right - */
	glm::vec3 rightDirection = glm::cross(m_Direction, upDirection);

	float speed = 5.0f;

	/* - Forward and Back movement - */
	if (Input::IsKeyDown(KeyCode::W))
	{
		/* - Incrementing current Pos with camera direction and constants*/
		m_Position += m_Direction * speed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::S))
	{
		m_Position -= m_Direction * speed * ts;
		moved = true;
	}
	/* - Left and Right Movement - */
	if (Input::IsKeyDown(KeyCode::A))
	{
		m_Position -= rightDirection * speed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::D))
	{
		m_Position += rightDirection * speed * ts;
		moved = true;
	}
	/* - Up and Down Movement - */
	if (Input::IsKeyDown(KeyCode::Space))
	{
		m_Position += upDirection * speed * ts;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::LeftShift))
	{
		m_Position -= upDirection * speed * ts;
		moved = true;
	}

	/* - Camera rotations with mouse - */
	if (delta.x != 0.0f || delta.y != 0.0f)
	{
		float pitch = delta.y * GetRotationSpeed();
		float yaw	= delta.x * GetRotationSpeed();

		/* - rotation quaternion - */
		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitch, rightDirection),
			glm::angleAxis(-yaw, glm::vec3(0.0f, 1.0f, 0.0f))));
		/* - new forward direction = old direction * quaternion - */
		m_Direction = glm::rotate(q, m_Direction);

		moved = true;
	}

	/* - Chaching old values if not moving - */

	if (moved)
	{
		RecalculateViewM();
		RecalculateRayDirections();
	}
}
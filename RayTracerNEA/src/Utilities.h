#pragma once

#include "Walnut/Random.h"

#define PI 3.1415926535897932385

#define NEAR_EPSILON	1e-3f
#define FLOAT_LARGE		1e+30f

#define BASE_SKY_COLOUR glm::vec3{ 0.0189f, 0.294f, 0.630f }
#define BLACK			glm::vec3{  0.0f, 0.0f, 0.0f }

namespace Utilities
{

	static uint32_t ToRGBA(const glm::vec4& color)
	{
		/* - Red, Green, Blue, Alpha channels - */

		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		/* Bitwise Manipulation, Left Shifts to correct positions 
		   since alpha needs to be in the most significant bit while
		   red in the last as for 0x(alpha)(blue)(green)(red) : 0xff00ff00
		*/
		uint32_t RGBAColor = (a << 24) | (b << 16) | (g << 8) | r;
		return RGBAColor;
	}

	static bool nearZero(glm::vec3 v) {
		const auto s = 1e-8;
		return ((fabs(v.x) < 0) && (fabs(v.y) < s) && (fabs(v.z) < s));
	}
}


/**
@file    light.hpp
@author  weizhen.tan@digipen.edu
@date    25/05/2025

This file contains the Light component.

*//*__________________________________________________________________________*/

#ifndef LIGHT_COMP_HPP
#define LIGHT_COMP_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <math.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

enum LIGHT_TYPE
{
	LIGHT_POINT,
	LIGHT_DIRECTIONAL,
	LIGHT_SPOT,
	LIGHT_TYPE_TOTAL
};

struct Light
{
	LIGHT_TYPE type;
	vec3 color = vec3(1.f);
	float intensity = 0.4f;
	float range; ///< if point/spot
	float angle; ///< if spot
};

static std::string to_string(LIGHT_TYPE _type)
{
	switch (_type)
	{
	case LIGHT_POINT:		return "Point";
	case LIGHT_DIRECTIONAL: return "Directional";
	case LIGHT_SPOT:		return "Spot";
	}
	return "";
}

#endif /* LIGHT_COMP_HPP */
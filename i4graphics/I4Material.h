#pragma once

#include "I4Vector4.h"
#include "I4Hash.h"
using namespace i4core;

namespace i4graphics {

	struct I4Material
	{
		I4Material()
		: ambient(0.58823f)
		, specularLevel(1.0f)
		, specularPower(32.0f)
		, twoSide(false)
		{
		}

		float			ambient;
		float			specularLevel;
		float	  		specularPower;
		I4Hash			diffuseMap;
		I4Hash			specularMap;
		I4Hash			normalMap;
		bool			twoSide;
	};
}
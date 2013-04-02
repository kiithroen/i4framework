#pragma once

#include "I4Vector4.h"
#include "I4Hash.h"
using namespace i4core;

namespace i4graphics {

	struct Material
	{
		Material()
		: ambient(0.58823f)
		, specularLevel(1.0f)
		, specularPower(32.0f)
		, twoSide(false)
		{
		}

		float			ambient;
		float			specularLevel;
		float	  		specularPower;
		Hash			diffuseMap;
		Hash			specularMap;
		Hash			normalMap;
		bool			twoSide;
	};
}
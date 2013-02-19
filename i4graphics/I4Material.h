#pragma once

#include "I4Vector4.h"
#include "I4Hash.h"
using namespace i4core;

namespace i4graphics {

	class I4Material
	{
	public:
		I4Material();

		void setDiffuse(unsigned char r, unsigned char g, unsigned char b);
		void setAmbient(unsigned char r, unsigned char g, unsigned char b);
		void setSpecular(unsigned char r, unsigned char g, unsigned char b);
		void setEmissive(unsigned char r, unsigned char g, unsigned char b);
		void setPower(float p);

		I4Vector4		diffuse;
		I4Vector4		ambient;
		I4Vector4		specular;
		I4Vector4		emissive;
		float	  		power;
		float			glossiness;

		I4Hash			diffuseMap;
		I4Hash			specularMap;
		I4Hash			normalMap;
	};
}
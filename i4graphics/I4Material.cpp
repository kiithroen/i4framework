#include "stdafx.h"
#include "I4Material.h"

namespace i4graphics
{

	I4Material::I4Material()
		: diffuseMap(nullptr)
		, normalMap(nullptr)
		, specularMap(nullptr)
		, power(32.0f)
		, glossiness(1.0f)
	{
		diffuse.r = 0.82f;
		diffuse.g = 0.82f;
		diffuse.b = 0.82f;
		diffuse.a = 1.0f;

		ambient.r = 0.22f;
		ambient.g = 0.22f;
		ambient.b = 0.22f;
		ambient.a = 1.0f;

		specular.r = 0.9f;
		specular.g = 0.9f;
		specular.b = 0.9f;
		specular.a = 1.0f;

		emissive.r = 0.0f;
		emissive.g = 0.0f;
		emissive.b = 0.0f;
		emissive.a = 1.0f;
	}


	void I4Material::setDiffuse(unsigned char r, unsigned char g, unsigned char b)
	{
		float inv = 1.0f / 255.0f;

		diffuse.r = r*inv;
		diffuse.g = g*inv;
		diffuse.b = b*inv;
		diffuse.a = 1.0f;
	}

	void I4Material::setAmbient(unsigned char r, unsigned char g, unsigned char b)
	{
		float inv = 1.0f / 255.0f;

		ambient.r = r*inv;
		ambient.g = g*inv;
		ambient.b = b*inv;
		ambient.a = 1.0f;
	}

	void I4Material::setSpecular(unsigned char r, unsigned char g, unsigned char b)
	{
		float inv = 1.0f / 255.0f;

		specular.r = r*inv;
		specular.g = g*inv;
		specular.b = b*inv;
		specular.a = 1.0f;
	}

	void I4Material::setEmissive(unsigned char r, unsigned char g, unsigned char b)
	{
		float inv = 1.0f / 255.0f;

		emissive.r = r*inv;
		emissive.g = g*inv;
		emissive.b = b*inv;
		emissive.a = 1.0f;
	}

}
#pragma once

#include "I4Quaternion.h"
using namespace i4core;

namespace i4graphics {

	struct KeyRotation
	{
		Quaternion	rotation;
		float			frame;
	};

	struct KeyPosition
	{
		Vector3	position;
		float		frame;
	};

	struct KeyScale
	{
		Vector3	scale;
		float		frame;
	};

	struct KeyFrameSet
	{
		typedef vector<KeyRotation>		KeyRotationVector;
		typedef vector<KeyPosition>		KeyPositionVector;
		typedef vector<KeyScale>			KeyScaleVector;

		string				nodeName;
		float				startFrame;
		float				endFrame;
		KeyRotationVector	vecKeyRotation;
		KeyPositionVector	vecKeyPosition;
		KeyScaleVector	vecKeyScale;
	};

}
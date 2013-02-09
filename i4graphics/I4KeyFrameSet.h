#pragma once

#include "I4Quaternion.h"
using namespace i4core;

namespace i4graphics {

	struct KeyRotation
	{
		float			frame;
		I4Quaternion	rotation;
	};

	struct KeyPosition
	{
		float		frame;
		I4Vector3		position;
	};

	struct KeyScale
	{
		float		frame;
		I4Vector3		scale;
	};

	struct KeyFrameSet
	{
		typedef vector<KeyRotation>		KeyRotationVector;
		typedef vector<KeyPosition>		KeyPositionVector;
		typedef vector<KeyScale>		KeyScaleVector;

		string				nodeName;
		float				startFrame;
		float				endFrame;
		KeyRotationVector	vecKeyRotation;
		KeyPositionVector	vecKeyPosition;
		KeyScaleVector		vecKeyScale;
	};

}
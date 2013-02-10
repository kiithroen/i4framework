#pragma once

#include "I4Quaternion.h"
using namespace i4core;

namespace i4graphics {

	struct I4KeyRotation
	{
		I4Quaternion	rotation;
		float			frame;
	};

	struct I4KeyPosition
	{
		I4Vector3	position;
		float		frame;
	};

	struct I4KeyScale
	{
		I4Vector3	scale;
		float		frame;
	};

	struct I4KeyFrameSet
	{
		typedef vector<I4KeyRotation>		I4KeyRotationVector;
		typedef vector<I4KeyPosition>		I4KeyPositionVector;
		typedef vector<I4KeyScale>			I4KeyScaleVector;

		string				nodeName;
		float				startFrame;
		float				endFrame;
		I4KeyRotationVector	vecKeyRotation;
		I4KeyPositionVector	vecKeyPosition;
		I4KeyScaleVector	vecKeyScale;
	};

}
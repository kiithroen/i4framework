#pragma once

#include "I4KeyFrameSet.h"

namespace i4graphics {

	class AnimationTrack
	{
	public:
		AnimationTrack(KeyFrameSet* keyFrameSet);
		~AnimationTrack();

		void 				advanceFrame(float deltaSec);
		void				resetStartFrame();

		bool 				getKeyRotation(I4Quaternion& out) const;	
		bool 				getKeyPosition(I4Vector3& out) const;
		bool 				getKeyScale(I4Vector3& out) const;

	private:
		KeyFrameSet*		keyFrameSet;
		float				currentFrame;

		static const float	ANIMATION_FRAME_RATE;
	};

}
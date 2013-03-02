#pragma once

#include "I4KeyFrameSet.h"

namespace i4graphics {

	class I4AnimationTrack
	{
	public:
		I4AnimationTrack(I4KeyFrameSet* keyFrameSet);
		~I4AnimationTrack();

		void 				advanceFrame(float dt);
		void				resetStartFrame();

		bool 				getKeyRotation(I4Quaternion& out) const;	
		bool 				getKeyPosition(I4Vector3& out) const;
		bool 				getKeyScale(I4Vector3& out) const;

	private:
		I4KeyFrameSet*		keyFrameSet;
		float				currentFrame;

		static const float	ANIMATION_FRAME_RATE;
	};

}
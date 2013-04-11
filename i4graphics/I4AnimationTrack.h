#pragma once

#include "I4KeyFrameSet.h"

namespace i4graphics {

	class AnimationTrack
	{
	public:
		AnimationTrack(KeyFrameSet* keyFrameSet, float start, float end);
		~AnimationTrack();

		void 				advanceFrame(float dt);
		void				resetStartFrame();

		bool 				getKeyRotation(Quaternion& out) const;	
		bool 				getKeyPosition(Vector3& out) const;
		bool 				getKeyScale(Vector3& out) const;

	private:
		KeyFrameSet*		keyFrameSet;
		float				currentFrame;
		float				startFrame;
		float				endFrame;

		static const float	ANIMATION_FRAME_RATE;
	};

}
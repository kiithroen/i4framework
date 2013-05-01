#pragma once

namespace i4core {
	class Vector3;
	class Quaternion;
}
using namespace i4core;

namespace i4graphics {

	struct KeyFrameSet;
	enum ANIMATION_PLAY_TYPE;

	class AnimationTrack
	{
	public:
		AnimationTrack(KeyFrameSet* keyFrameSet, float start, float end, ANIMATION_PLAY_TYPE type);
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
		ANIMATION_PLAY_TYPE	playType;

		static const float	ANIMATION_FRAME_RATE;
	};

}
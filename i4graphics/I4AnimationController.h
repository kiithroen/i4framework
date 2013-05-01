#pragma once

#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics {

	struct KeyFrameSet;
	class AnimationTrack;
	enum ANIMATION_PLAY_TYPE;

	class AnimationController
	{
		typedef map<string, AnimationTrack*>		AnimationTrackMap;

	public:
		AnimationController();
		~AnimationController();

		void				addTrack(const char* name, KeyFrameSet* keyFrameSet, float start, float end, ANIMATION_PLAY_TYPE type);
		void				playTrack(const char* name);

		void				animate(float dt);

		const Matrix4x4&	getAnimationTM() const		{ return animationTM; }

	private:
		AnimationTrackMap	mapAnimationTrack;
		Matrix4x4			animationTM;
		AnimationTrack*		curAnimationTrack;
	};

}
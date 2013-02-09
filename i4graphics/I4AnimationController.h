#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics {

	struct KeyFrameSet;
	class AnimationTrack;

	class AnimationController
	{
		typedef map<string, AnimationTrack*>		AnimationTrackMap;

	public:
		AnimationController();
		~AnimationController();

		void				addTrack(const char* name, KeyFrameSet* keyFrameSet);
		void				playTrack(const char* name);

		void				animate(float deltaSec);

		const I4Matrix4x4&	getAnimationTM() const		{ return animationTM; }

	private:
		AnimationTrackMap	mapAnimationTrack;
		I4Matrix4x4			animationTM;
		AnimationTrack*		curAnimationTrack;
	};

}
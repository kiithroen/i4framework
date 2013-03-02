#pragma once

#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics {

	struct I4KeyFrameSet;
	class I4AnimationTrack;

	class I4AnimationController
	{
		typedef map<string, I4AnimationTrack*>		I4AnimationTrackMap;

	public:
		I4AnimationController();
		~I4AnimationController();

		void				addTrack(const char* name, I4KeyFrameSet* keyFrameSet);
		void				playTrack(const char* name);

		void				animate(float dt);

		const I4Matrix4x4&	getAnimationTM() const		{ return animationTM; }

	private:
		I4AnimationTrackMap	mapAnimationTrack;
		I4Matrix4x4			animationTM;
		I4AnimationTrack*	curAnimationTrack;
	};

}
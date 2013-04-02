#include "stdafx.h"
#include "I4AnimationController.h"
#include "I4AnimationTrack.h"

namespace i4graphics
{

	AnimationController::AnimationController()
		: curAnimationTrack(nullptr)
	{
		animationTM.makeIdentity();
	}

	AnimationController::~AnimationController()
	{
		for (auto& itr : mapAnimationTrack)
		{
			delete itr.second;
		}
		mapAnimationTrack.clear();
	}

	void AnimationController::animate(float dt)
	{		
		if (curAnimationTrack == nullptr)
			return;

		curAnimationTrack->advanceFrame(dt);	

		Quaternion q;
		if (curAnimationTrack->getKeyRotation(q) == true)
		{
			q.extractRotationMatrix(animationTM);
		}

		Vector3 v;
		if (curAnimationTrack->getKeyPosition(v) == true)
		{
			animationTM._41 = v.x;
			animationTM._42 = v.y;
			animationTM._43 = v.z;
		}

		if (curAnimationTrack->getKeyScale(v) == true)
		{
			Matrix4x4 m;
			m.makeScale(v.x, v.y, v.z);
			animationTM = m*animationTM;
		}
	}

	void AnimationController::addTrack(const char* name, KeyFrameSet* keyFrameSet)
	{
		auto animationTrack = new AnimationTrack(keyFrameSet);
		mapAnimationTrack.insert(make_pair(name, animationTrack));	
	}

	void AnimationController::playTrack(const char* name)
	{
		auto itr = mapAnimationTrack.find(name);
		if (itr == mapAnimationTrack.end())
		{
			curAnimationTrack = nullptr;
			return;
		}

		curAnimationTrack = itr->second;
		curAnimationTrack->resetStartFrame();
	}
}
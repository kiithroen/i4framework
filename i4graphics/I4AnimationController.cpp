#include "stdafx.h"
#include "I4AnimationController.h"
#include "I4AnimationTrack.h"

namespace i4graphics
{

	I4AnimationController::I4AnimationController()
		: curAnimationTrack(nullptr)
	{
		animationTM.makeIdentity();
	}

	I4AnimationController::~I4AnimationController()
	{
		for (auto& itr : mapAnimationTrack)
		{
			delete itr.second;
		}
		mapAnimationTrack.clear();
	}

	void I4AnimationController::animate(float deltaSec)
	{		
		if (curAnimationTrack == nullptr)
			return;

		curAnimationTrack->advanceFrame(deltaSec);	

		I4Quaternion q;
		if (curAnimationTrack->getKeyRotation(q) == true)
		{
			q.extractRotationMatrix(animationTM);
		}

		I4Vector3 v;
		if (curAnimationTrack->getKeyPosition(v) == true)
		{
			animationTM._41 = v.x;
			animationTM._42 = v.y;
			animationTM._43 = v.z;
		}

		if (curAnimationTrack->getKeyScale(v) == true)
		{
			animationTM._11 *= v.x;
			animationTM._22 *= v.y;
			animationTM._33 *= v.z;
		}
	}

	void I4AnimationController::addTrack(const char* name, I4KeyFrameSet* keyFrameSet)
	{
		I4AnimationTrack* animationTrack = new I4AnimationTrack(keyFrameSet);
		mapAnimationTrack.insert(make_pair(name, animationTrack));	
	}

	void I4AnimationController::playTrack(const char* name)
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
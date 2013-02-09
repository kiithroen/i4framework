#include "I4AnimationController.h"
#include "I4AnimationTrack.h"

namespace i4graphics
{

	AnimationController::AnimationController()
		: curAnimationTrack(NULL)
	{
		animationTM.makeIdentity();
	}

	AnimationController::~AnimationController()
	{
		for (AnimationTrackMap::iterator itr = mapAnimationTrack.begin(); itr != mapAnimationTrack.end(); ++itr)
		{
			delete itr->second;
		}
		mapAnimationTrack.clear();
	}

	void AnimationController::animate(float deltaSec)
	{		
		if (curAnimationTrack == NULL)
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

	void AnimationController::addTrack(const char* name, KeyFrameSet* keyFrameSet)
	{
		AnimationTrack* animationTrack = new AnimationTrack(keyFrameSet);
		mapAnimationTrack.insert(make_pair(name, animationTrack));	
	}

	void AnimationController::playTrack(const char* name)
	{
		AnimationTrackMap::iterator itr = mapAnimationTrack.find(name);
		if (itr == mapAnimationTrack.end())
		{
			curAnimationTrack = NULL;
			return;
		}

		curAnimationTrack = itr->second;
		curAnimationTrack->resetStartFrame();
	}
}
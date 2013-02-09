#include "I4AnimationTrack.h"
#include "I4KeyFrameSet.h"
#include "I4Log.h"

namespace i4graphics
{

	template <typename T>
	void getPairKeys(float frame, vector<T>& keys, T*& k1, T*& k2)
	{
		if (keys.size() > 1)
		{
			int		mkey;
			int		lkey = 0;
			int		rkey = (int)keys.size()-1;

			if (frame > keys[rkey].frame)
			{
				k1 = &keys[rkey];
				k2 = &keys[rkey];
				return;
			}

			if (frame < keys[lkey].frame)
			{
				k1 = &keys[lkey];
				k2 = &keys[lkey];
				return;
			}

			while (rkey >= lkey)
			{
				mkey = (rkey + lkey)/2;
				if (keys[mkey].frame <= frame && keys[mkey+1].frame >= frame)
				{
					k1 = &keys[mkey];
					k2 = &keys[mkey+1];
					return;
				}
				if (keys[mkey].frame > frame)
					rkey = mkey;
				else
					lkey = mkey;
			}
		}
		else if (keys.size() == 1)
		{
			k1 = &keys[0];
			k2 = &keys[0];
			return;
		}
		else	//  if (keys.size() == 0)
		{
			k1 = k2 = NULL;
			return;
		}
	}

	const float AnimationTrack::ANIMATION_FRAME_RATE = 30.0f;

	AnimationTrack::AnimationTrack(KeyFrameSet* _keyFrameSet)
		: keyFrameSet(_keyFrameSet)
		, currentFrame(0)
	{
	}

	AnimationTrack::~AnimationTrack()
	{
	}


	void AnimationTrack::advanceFrame(float deltaSec)
	{
		currentFrame += deltaSec*ANIMATION_FRAME_RATE;

		if (currentFrame >= keyFrameSet->endFrame)
		{
			currentFrame = keyFrameSet->startFrame;
		}
	}

	void AnimationTrack::resetStartFrame()
	{
		currentFrame = keyFrameSet->startFrame;
	}

	bool AnimationTrack::getKeyRotation(I4Quaternion& out) const
	{
		KeyRotation* key1 = NULL;
		KeyRotation* key2 = NULL;
		getPairKeys(currentFrame, keyFrameSet->vecKeyRotation, key1, key2);

		if (key1 == NULL || key2 == NULL)
		{
			return false;
		}

		if (key1 == key2)
		{
			out = key1->rotation;
		}
		else
		{
			float t = I4MathUtil::percent(key1->frame, key2->frame, currentFrame);
			I4Quaternion::slerp(out, key1->rotation, key2->rotation, t);
		}

		return true;
	}

	bool AnimationTrack::getKeyPosition(I4Vector3& out) const
	{
		KeyPosition* key1 = NULL;
		KeyPosition* key2 = NULL;
		getPairKeys(currentFrame, keyFrameSet->vecKeyPosition, key1, key2);

		if (key1 == NULL || key2 == NULL)
		{
			return false;
		}

		if (key1 == key2)
		{
			out = key1->position;
		}
		else
		{
			float t = I4MathUtil::percent(key1->frame, key2->frame, currentFrame);
			out = I4MathUtil::lerp(key1->position, key2->position, t);
		}

		return true;
	}

	bool AnimationTrack::getKeyScale(I4Vector3& out) const
	{
		KeyScale* key1 = NULL;
		KeyScale* key2 = NULL;

		getPairKeys(currentFrame, keyFrameSet->vecKeyScale, key1, key2);

		if (key1 == NULL || key2 == NULL)
		{
			return false;
		}

		if (key1 == key2)
		{
			out = key1->scale;
		}
		else
		{
			float t = I4MathUtil::percent(key1->frame, key2->frame, currentFrame);
			out = I4MathUtil::lerp(key1->scale, key2->scale, t);
		}

		return true;
	}

}
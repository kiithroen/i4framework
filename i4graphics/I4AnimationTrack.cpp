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
			k1 = k2 = nullptr;
			return;
		}
	}

	const float I4AnimationTrack::ANIMATION_FRAME_RATE = 30.0f;

	I4AnimationTrack::I4AnimationTrack(I4KeyFrameSet* _keyFrameSet)
		: keyFrameSet(_keyFrameSet)
		, currentFrame(0)
	{
	}

	I4AnimationTrack::~I4AnimationTrack()
	{
	}


	void I4AnimationTrack::advanceFrame(float deltaSec)
	{
		currentFrame += deltaSec*ANIMATION_FRAME_RATE;

		if (currentFrame >= keyFrameSet->endFrame)
		{
			currentFrame = keyFrameSet->startFrame;
		}
	}

	void I4AnimationTrack::resetStartFrame()
	{
		currentFrame = keyFrameSet->startFrame;
	}

	bool I4AnimationTrack::getKeyRotation(I4Quaternion& out) const
	{
		I4KeyRotation* key1 = nullptr;
		I4KeyRotation* key2 = nullptr;
		getPairKeys(currentFrame, keyFrameSet->vecKeyRotation, key1, key2);

		if (key1 == nullptr || key2 == nullptr)
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

	bool I4AnimationTrack::getKeyPosition(I4Vector3& out) const
	{
		I4KeyPosition* key1 = nullptr;
		I4KeyPosition* key2 = nullptr;
		getPairKeys(currentFrame, keyFrameSet->vecKeyPosition, key1, key2);

		if (key1 == nullptr || key2 == nullptr)
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

	bool I4AnimationTrack::getKeyScale(I4Vector3& out) const
	{
		I4KeyScale* key1 = nullptr;
		I4KeyScale* key2 = nullptr;

		getPairKeys(currentFrame, keyFrameSet->vecKeyScale, key1, key2);

		if (key1 == nullptr || key2 == nullptr)
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
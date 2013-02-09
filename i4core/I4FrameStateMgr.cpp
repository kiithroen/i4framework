#include "I4FrameStateMgr.h"
#include "I4FrameState.h"

namespace i4core {

	I4FrameStateMgr::I4FrameStateMgr()
	: curFrameState(nullptr)
	, nextFrameStateID("")
	{
	}

	I4FrameStateMgr::~I4FrameStateMgr()
	{
		if (curFrameState)
		{
			curFrameState->onExit();
		}

		for (auto &itr : frameStateMap)
		{
			delete itr.second;
		}

		frameStateMap.clear();
	}

	bool I4FrameStateMgr::onUpdate(float deltaSec)
	{
		if (nextFrameStateID != "")
		{
			if (changeFrameState(nextFrameStateID.c_str()) == false)
				return false;

			nextFrameStateID = "";
		}

		if (curFrameState)
		{
			return curFrameState->onUpdate(deltaSec);
		}

		return false;
	}

	bool I4FrameStateMgr::onRender(float deltaSec)
	{
		if (curFrameState)
		{
			return curFrameState->onRender(deltaSec);
		}

		return false;
	}

	void I4FrameStateMgr::onKeyDown(unsigned int key)
	{
		if (curFrameState)
		{
			curFrameState->onKeyDown(key);
		}
	}

	void I4FrameStateMgr::onKeyUp(unsigned int key)
	{
		if (curFrameState)
		{
			curFrameState->onKeyUp(key);
		}
	}

	void I4FrameStateMgr::onMouseMove(unsigned int x, unsigned int y)
	{
		if (curFrameState)
		{
			curFrameState->onMouseMove(x, y);
		}
	}

	void I4FrameStateMgr::onLButtonDown(unsigned int x, unsigned int y)
	{
		if (curFrameState)
		{
			curFrameState->onLButtonDown(x, y);
		}
	}

	void I4FrameStateMgr::onLButtonUp(unsigned int x, unsigned int y)
	{
		if (curFrameState)
		{
			curFrameState->onLButtonUp(x, y);
		}
	}

	void I4FrameStateMgr::onRButtonDown(unsigned int x, unsigned int y)
	{
		if (curFrameState)
		{
			curFrameState->onRButtonDown(x, y);
		}
	}

	void I4FrameStateMgr::onRButtonUp(unsigned int x, unsigned int y)
	{
		if (curFrameState)
		{
			curFrameState->onRButtonUp(x, y);
		}
	}

	bool I4FrameStateMgr::addFrameState(const char* id, I4FrameState* state)
	{
		if (state == nullptr)
			return false;

		auto itr = frameStateMap.find(id);
		if (itr != frameStateMap.end())	// 중복 추가면 실패
			return false;

		state->frameStateID = id;
		state->frameStateMgr = this;
		frameStateMap.insert(make_pair(id, state));

		return true;
	}

	bool I4FrameStateMgr::changeFrameState(const char* id)
	{
		auto itr = frameStateMap.find(id);
		if (itr == frameStateMap.end())	// 없는 상태면 실패
			return false;

		if (curFrameState)
		{
			curFrameState->onExit();
		}		

		curFrameState = itr->second;
		assert(curFrameState != nullptr);

		return curFrameState->onEnter();
	}


	const char* I4FrameStateMgr::getCurFrameStateID()
	{
		if (curFrameState)
		{
			return curFrameState->getFrameStateID();
		}

		return "";
	}

}
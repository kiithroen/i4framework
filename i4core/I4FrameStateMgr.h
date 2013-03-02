#pragma once

#include "i4core.h"

namespace i4core
{
	class I4FrameCallback;
	class I4FrameState;

	class I4CORE_API I4FrameStateMgr
	{
		friend class I4FrameCallback;
		typedef map<string, I4FrameState*>	I4FrameStateMap;
	public:
		I4FrameStateMgr();
		virtual ~I4FrameStateMgr();

	public:
		bool			onUpdate(float dt);
		bool			onRender(float dt);

		void			onKeyDown(unsigned int key);
		void			onKeyUp(unsigned int key);	

		void			onMouseMove(unsigned int x, unsigned int y);

		void			onLButtonDown(unsigned int x, unsigned int y);
		void			onLButtonUp(unsigned int x, unsigned int y);

		void			onRButtonDown(unsigned int x, unsigned int y);
		void			onRButtonUp(unsigned int x, unsigned int y);

		bool			addFrameState(const char* id, I4FrameState* state);
		const char*		getCurFrameStateID();

		void			nextFrameState(const char* id)	{ nextFrameStateID = id; }
		bool			changeFrameState(const char* id);

	private:
		I4FrameState*		curFrameState;
		I4FrameStateMap		frameStateMap;
		string			nextFrameStateID;
	};
}
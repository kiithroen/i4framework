#pragma once

#include "i4core.h"

namespace i4core
{
	class I4FrameStateMgr;
	class I4FrameState
	{
		friend class I4FrameStateMgr;
	public:
		I4FrameState() : frameStateMgr(nullptr)	{}
		virtual ~I4FrameState()				{}

		const char*		getFrameStateID()								{ return frameStateID.c_str(); }

		virtual bool	onEnter()										{ return true; }
		virtual	bool	onUpdate(float dt)								{ return true; }
		virtual bool 	onRender(float dt)								{ return true; }
		virtual void	onExit()										{}

		virtual void	onKeyDown(unsigned int key)						{}
		virtual void	onKeyUp(unsigned int key)						{}
		
		virtual void	onMouseMove(unsigned int x, unsigned int y)		{}

		virtual void	onLButtonDown(unsigned int x, unsigned int y)	{}
		virtual void	onLButtonUp(unsigned int x, unsigned int y)		{}

		virtual void	onRButtonDown(unsigned int x, unsigned int y)	{}
		virtual void	onRButtonUp(unsigned int x, unsigned int y)		{}

	protected:
		I4FrameStateMgr*	frameStateMgr;
		string			frameStateID;
	};
}
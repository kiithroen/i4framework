#pragma once

#include "i4core.h"

namespace i4core
{
	class I4Framework;

	class I4CORE_API I4FrameCallback
	{
	public:
		I4FrameCallback()			{}
		virtual ~I4FrameCallback()	{}

		virtual	bool	onStart()										{ return true; }
		virtual void	onEnd()											{}

		virtual bool 	onUpdate(float dt)						{ return true; }
		virtual bool 	onRender(float dt)						{ return true; }

		virtual void	onKeyDown(unsigned int key)						{}
		virtual void	onKeyUp(unsigned int key)						{}

		virtual void	onMouseMove(unsigned int x, unsigned int y)		{}

		virtual void	onLButtonDown(unsigned int x, unsigned int y)	{}
		virtual void	onLButtonUp(unsigned int x, unsigned int y)		{}

		virtual void	onRButtonDown(unsigned int x, unsigned int y)	{}
		virtual void	onRButtonUp(unsigned int x, unsigned int y)		{}
	};
}
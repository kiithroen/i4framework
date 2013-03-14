#pragma once

#include "i4core.h"

namespace i4core
{
	class I4Framework;
	struct I4InputState;

	class I4FrameCallback
	{
	public:
		I4FrameCallback()			{}
		virtual ~I4FrameCallback()	{}

		virtual	bool	onStart()								{ return true; }
		virtual void	onEnd()									{}

		virtual bool 	onTick(float dt)						{ return true; }
		virtual bool	onUpdate()								{ return true; }
		virtual bool 	onRender()								{ return true; }

		virtual void	onInput(const I4InputState& state)		{}
	};
}
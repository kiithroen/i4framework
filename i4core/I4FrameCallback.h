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

		virtual bool 	onUpdate(float dt)						{ return true; }
		virtual bool 	onRender(float dt)						{ return true; }

		virtual void	onInput(const I4InputState& state)		{}
	};
}
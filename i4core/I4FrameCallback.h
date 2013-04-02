#pragma once

#include "i4core.h"

namespace i4core
{
	class Framework;
	struct InputState;

	class FrameCallback
	{
	public:
		FrameCallback()			{}
		virtual ~FrameCallback()	{}

		virtual	bool	onStart()								{ return true; }
		virtual void	onEnd()									{}

		virtual bool 	onTick(float dt)						{ return true; }
		virtual bool	onUpdate()								{ return true; }
		virtual bool 	onRender()								{ return true; }

		virtual void	onInput(const InputState& state)		{}
	};
}
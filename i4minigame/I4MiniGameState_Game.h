#pragma once

#include "I4FrameState.h"
using namespace i4core;

class I4MiniGameState_Game : public I4FrameState
{
public:
	I4MiniGameState_Game();
	virtual ~I4MiniGameState_Game();

	virtual	bool	onEnter();
	virtual void	onExit();

	virtual bool	onUpdate(float dt);
	virtual bool	onRender(float dt);
	virtual void	onLButtonDown(unsigned int x, unsigned int y);
};

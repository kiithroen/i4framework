#pragma once

#include "I4FrameCallback.h"

using namespace i4core;

namespace i4core
{
	class I4FrameStateMgr;
	class I4Camera;
}
using namespace i4core;

namespace i4graphics
{
	class I4DeferredRenderer;
	class I4Actor;
	class I4ActorMgr;
}
using namespace i4graphics;

class I4MiniGameFrameCallback : public I4FrameCallback
{
public:
	I4MiniGameFrameCallback();
	virtual ~I4MiniGameFrameCallback();

	virtual	bool	onStart();
	virtual void	onEnd();

	virtual bool	onUpdate(float delta);	
	virtual bool	onRender(float delta);

	virtual void	onKeyDown(unsigned int key);
	virtual void	onKeyUp(unsigned int key);	

	virtual void	onMouseMove(unsigned int x, unsigned int y);

	virtual void	onLButtonDown(unsigned int x, unsigned int y);
	virtual void	onLButtonUp(unsigned int x, unsigned int y);

	virtual void	onRButtonDown(unsigned int x, unsigned int y);
	virtual void	onRButtonUp(unsigned int x, unsigned int y);

private:
	void	updateCamera(float deltaTime);
	void	commitToRenderer(float deltaTime);

private:
	I4FrameStateMgr*	frameStateMgr;
	I4DeferredRenderer*	renderer;
	I4Camera*			camera;
	int					prevMouseX;
	int					prevMouseY;
	bool				isRButtonDown;
	float				camYaw;
	float				camPitch;
	float				camRoll;

	I4Actor*			actor[100];
	I4ActorMgr*			actorMgr;
};

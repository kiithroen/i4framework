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
	class I4Renderer;
	class I4ModelMgr;
}
using namespace i4graphics;

namespace i4object
{
	class I4ObjectMgr;
	class I4ObjectNode;
	class I4BulletPhysics;
}
using namespace i4object;

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
	void	updateCamera(float dt);
	void	commitToRenderer(float dt);

private:
	I4FrameStateMgr*	frameStateMgr;
	I4ObjectMgr*		objectMgr;
	I4Renderer*			renderer;
	I4Camera*			camera;
	I4ModelMgr*			modelMgr;
	I4BulletPhysics*	bulletPhysics;

	int					prevMouseX;
	int					prevMouseY;
	bool				isRButtonDown;
	float				camYaw;
	float				camPitch;
	float				camRoll;

	I4ObjectNode*		nodeLight[100];
};

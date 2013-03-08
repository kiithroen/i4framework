#pragma once

#include "I4FrameCallback.h"

using namespace i4core;

namespace i4core
{
	class I4Camera;
}
using namespace i4core;

namespace i4graphics
{
	class I4Renderer;
	class I4ModelMgr;

	class I4LineMesh;
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

	virtual	bool	onStart() override;
	virtual void	onEnd() override;

	virtual bool	onUpdate(float dt) override;	
	virtual bool	onRender(float dt) override;

	virtual void	onInput(const I4InputState& state) override;

private:
	void	updateCamera(float dt);

private:
	I4ObjectMgr*		objectMgr;
	I4Renderer*			renderer;
	I4ObjectNode*		player;
	I4ModelMgr*			modelMgr;
	I4BulletPhysics*	bulletPhysics;
	I4ObjectNode*		nodeLight[100];
};

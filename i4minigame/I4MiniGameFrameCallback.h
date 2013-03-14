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
	class I4Object;
	class I4PhysXMgr;
}
using namespace i4object;

class I4MiniGameFrameCallback : public I4FrameCallback
{
public:
	I4MiniGameFrameCallback();
	virtual ~I4MiniGameFrameCallback();

	virtual	bool	onStart() override;
	virtual void	onEnd() override;

	virtual bool	onTick(float dt) override;

	virtual bool	onUpdate() override;
	virtual bool	onRender() override;

	virtual void	onInput(const I4InputState& state) override;
	
private:
	I4ObjectMgr*		objectMgr;
	I4Renderer*			renderer;
	I4Object*		player;
	I4ModelMgr*			modelMgr;
	I4PhysXMgr*			physXMgr;
	I4Object*		objLight[100];

	I4Object*		spectator;
};

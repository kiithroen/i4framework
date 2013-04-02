#pragma once

#include "I4FrameCallback.h"

using namespace i4core;

namespace i4core
{
	class Camera;
}
using namespace i4core;

namespace i4graphics
{
	class Renderer;
	class ModelMgr;

	class LineMesh;
}
using namespace i4graphics;

namespace i4object
{
	class ObjectMgr;
	class Object;
	class PhysXMgr;
}
using namespace i4object;

class MiniGameFrameCallback : public FrameCallback
{
public:
	MiniGameFrameCallback();
	virtual ~MiniGameFrameCallback();

	virtual	bool	onStart() override;
	virtual void	onEnd() override;

	virtual bool	onTick(float dt) override;

	virtual bool	onUpdate() override;
	virtual bool	onRender() override;

	virtual void	onInput(const InputState& state) override;
	
private:
	ObjectMgr*		objectMgr;
	Renderer*			renderer;
	Object*		player;
	ModelMgr*			modelMgr;
	PhysXMgr*			physXMgr;
	Object*		objLight[100];

	Object*		spectator;
};

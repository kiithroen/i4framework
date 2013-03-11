#include "stdafx.h"
#include "I4ObjectCharacterComponent.h"
#include "I4Log.h"
#include "I4Hash.h"
#include "I4Profile.h"

namespace i4object {

	I4ObjectCharacterComponent::I4ObjectCharacterComponent(void)
		: controller(nullptr)
	{
	}


	I4ObjectCharacterComponent::~I4ObjectCharacterComponent(void)
	{
	}

	void I4ObjectCharacterComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(I4Hash("onPreSimulate"), this, bind(&I4ObjectCharacterComponent::onPreSimulate, this, _1));

		velocity = PxVec3(0, 0, 0.1f);
		gravity = PxVec3(0.0, -9.8f, 0.0);
	}

	void I4ObjectCharacterComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onPreSimulate"), this);
	}

	void I4ObjectCharacterComponent::attach(float radius, float height, float slopeLimit, float stepOffset)
	{
		I4Vector3 p;
		getOwner()->getWorldTM().extractTranslation(p);
		controller = getOwner()->getObjectMgr()->getPhysXMgr()->createCapsuleController(p, radius, height, slopeLimit, stepOffset, nullptr, nullptr);
	}

	void I4ObjectCharacterComponent::onPreSimulate(I4MessageArgs& args)
	{
		I4PROFILE_THISFUNC;

		float dt = args[0].asFloat();

		PxVec3 disp = velocity*dt;
		disp += gravity*dt;

		const PxU32 flag = controller->move(disp, 0.001f, dt, PxControllerFilters());

		const PxExtendedVec3 p = controller->getFootPosition();
		getOwner()->setLocalPosition(I4Vector3((float)p.x, (float)p.y, (float)p.z));
	}
}
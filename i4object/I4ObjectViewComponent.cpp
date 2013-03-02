#include "stdafx.h"
#include "I4ObjectViewComponent.h"
#include "I4Log.h"

namespace i4object {

	I4ObjectViewComponent::I4ObjectViewComponent(void)
	{
	}


	I4ObjectViewComponent::~I4ObjectViewComponent(void)
	{
	}

	void I4ObjectViewComponent::Test(const I4MessageArgs& args)
	{
		I4LOG_INFO << getOwner()->getName().c_str();
	}

	void I4ObjectViewComponent::onAdd()
	{
		getBroadcastMessenger().bind(0, this, bind(&I4ObjectViewComponent::Test, this, _1));
	}

	void I4ObjectViewComponent::onRemove()
	{
		getBroadcastMessenger().unbind(0, this);
	}

}
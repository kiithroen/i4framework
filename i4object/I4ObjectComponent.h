#pragma once

#include "I4ObjectNode.h"
#include "I4ObjectMgr.h"
using namespace i4core;

namespace i4object {
	

	class I4ObjectComponent
	{
	public:
		I4ObjectComponent(void)				{}
		virtual ~I4ObjectComponent(void)	{}

		virtual void				onAdd()						{}
		virtual void				onRemove()					{}

		I4ObjectNode*				getOwner()						{ return owner; }
		void						setOwner(I4ObjectNode* _owner)	{ owner = _owner; }

		I4ObjectComponentMessenger&	getBroadcastMessenger()			{ return owner->getObjectMgr()->getMessenger(); }
		I4ObjectComponentMessenger&	getDirectMessenger()			{ return owner->getMessenger(); }

	private:
		I4ObjectNode*		owner;
	};
}
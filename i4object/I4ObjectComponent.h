#pragma once

#include "I4ObjectNode.h"
#include "I4ObjectMgr.h"
using namespace i4core;

namespace i4object {
	

	class ObjectComponent
	{
	public:
		ObjectComponent(void)				{}
		virtual ~ObjectComponent(void)	{}

		virtual void				onAdd()						{}
		virtual void				onRemove()					{}

		Object*				getOwner()						{ return owner; }
		void						setOwner(Object* _owner)	{ owner = _owner; }

		ObjectComponentMessenger&	getBroadcastMessenger()			{ return owner->getObjectMgr()->getMessenger(); }
		ObjectComponentMessenger&	getDirectMessenger()			{ return owner->getMessenger(); }

	private:
		Object*		owner;
	};
}
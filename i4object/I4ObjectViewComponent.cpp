#include "stdafx.h"
#include "I4ObjectViewComponent.h"
#include "I4Log.h"
#include "I4ModelMgr.h"
#include "I4Model.h"

namespace i4object {

	ObjectViewComponent::ObjectViewComponent(void)
		: offset(MATRIX4X4_IDENTITY)
	{
	}


	ObjectViewComponent::~ObjectViewComponent(void)
	{
	}

	void ObjectViewComponent::onAdd()
	{
	}

	void ObjectViewComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(Hash("onAnimate"), this);
		getBroadcastMessenger().unsubscribe(Hash("onCommitToRenderer"), this);
	}

	bool ObjectViewComponent::attachModel(const char* name, const char* modelPrefixName, bool hasMesh, bool hasMtrl, bool hasBone)
	{
		ModelMgr* modelMgr = getOwner()->getObjectMgr()->getModelMgr();
		model = modelMgr->createModel(name);
		if (model == nullptr)
			return false;

		if (hasMesh)
		{
			string meshFile = modelPrefixName;
			meshFile = meshFile + ".mesh.xml";
			if (!modelMgr->attachMesh(model, meshFile.c_str()))
				return false;
		}

		if (hasMtrl)
		{
			string mtrlFile = modelPrefixName;
			mtrlFile = mtrlFile + ".mtrl.xml";
			if (!modelMgr->attachMaterial(model, mtrlFile.c_str()))
				return false;
		}

		if (hasBone)
		{
			string boneFile = modelPrefixName;
			boneFile = boneFile + ".bone.xml";
			if (!modelMgr->attachBone(model, boneFile.c_str()))
				return false;
		}

		model->initialize();

		getBroadcastMessenger().subscribe(Hash("onCommitToRenderer"), this, bind(&ObjectViewComponent::onCommitToRenderer, this, _1));

		return true;
	}

	bool ObjectViewComponent::attachAni(const char* fname, const char* aniName, float start, float end)
	{
		if (model == nullptr)
			return false;

		ModelMgr* modelMgr = getOwner()->getObjectMgr()->getModelMgr();
		if (!modelMgr->attachAni(model, fname, aniName, start, end))
			return false;

		getBroadcastMessenger().subscribe(Hash("onAnimate"), this, bind(&ObjectViewComponent::onUpateAnimation, this, _1));

		return true;
	}

	void ObjectViewComponent::playAnimation(const char* name, float speed)
	{
		if (model)
		{
			model->playAnimation(name, speed);
		}
	}

	void ObjectViewComponent::onUpateAnimation(MessageArgs& args)
	{
		if (model)
		{
			model->animate(args[0].asFloat());
		}
	}

	void ObjectViewComponent::onCommitToRenderer(MessageArgs& args)
	{
		if (model)
		{
			model->commitToRenderer(getOwner()->getObjectMgr()->getRenderer(), offset*getOwner()->getWorldTM());
		}
	}

	void ObjectViewComponent::setShadowCaster(bool enable)
	{
		model->setShadowCaster(enable);
	}

	void ObjectViewComponent::setShadowReceiver(bool enable)
	{
		model->setShadowReceiver(enable);
	}


}
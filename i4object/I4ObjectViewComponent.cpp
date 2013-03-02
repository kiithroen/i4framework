#include "stdafx.h"
#include "I4ObjectViewComponent.h"
#include "I4Log.h"
#include "I4ModelMgr.h"
#include "I4Model.h"

namespace i4object {

	I4ObjectViewComponent::I4ObjectViewComponent(void)
	{
	}


	I4ObjectViewComponent::~I4ObjectViewComponent(void)
	{
	}

	void I4ObjectViewComponent::onAdd()
	{
	}

	void I4ObjectViewComponent::onRemove()
	{
		getBroadcastMessenger().unbind(I4Hash("onAnimate"), this);
		getBroadcastMessenger().unbind(I4Hash("onRender"), this);
	}

	bool I4ObjectViewComponent::attachModel(const char* name, const char* modelPrefixName, bool hasMesh, bool hasMtrl, bool hasBone)
	{
		I4ModelMgr* modelMgr = getOwner()->getObjectMgr()->getModelMgr();
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

		getBroadcastMessenger().bind(I4Hash("onRender"), this, bind(&I4ObjectViewComponent::onRender, this, _1));

		return true;
	}

	bool I4ObjectViewComponent::attachAni(const char* fname, const char* aniName)
	{
		if (model == nullptr)
			return false;

		I4ModelMgr* modelMgr = getOwner()->getObjectMgr()->getModelMgr();
		if (!modelMgr->attachAni(model, fname, aniName))
			return false;

		getBroadcastMessenger().bind(I4Hash("onAnimate"), this, bind(&I4ObjectViewComponent::onAnimate, this, _1));

		return true;
	}

	void I4ObjectViewComponent::playAnimation(const char* name)
	{
		if (model)
		{
			model->playAnimation(name);
		}
	}

	void I4ObjectViewComponent::onAnimate(I4MessageArgs& args)
	{
		if (model)
		{
			model->animate(args[0].asFloat());
		}
	}

	void I4ObjectViewComponent::onRender(I4MessageArgs& args)
	{
		if (model)
		{
			model->render(getOwner()->getObjectMgr()->getRenderer(), getOwner()->getWorldTM());
		}
	}


}
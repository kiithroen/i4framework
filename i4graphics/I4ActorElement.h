#pragma once

#include "I4Mesh.h"
#include "I4ActorElementInfo.h"

namespace i4graphics {

	struct I4KeyFrameSet;

	class I4ShaderMgr;
	class I4Actor;
	class I4ActorBoneResource;
	class I4AnimationController;
	class I4DeferredRenderer;

	//------------------------- I4ActorElement ---------------------

	class I4ActorElement
	{
	public:
		I4ActorElement(I4Actor* actor, I4ActorElementInfo* info);
		virtual ~I4ActorElement();

		void					registerAni(const char* name, I4KeyFrameSet* keyFrameSet);
		void					playAni(const char* name);

		virtual bool			initialize();
		virtual void			animate(float deltaSec, const I4Matrix4x4& parentTM);
		virtual void			render(I4DeferredRenderer* renderer, const I4Matrix4x4& parentTM)	{}

		const I4Matrix4x4& 		getResultTM() const			{ return resultTM;	}

		int						getParentID() const			{ return elementInfo->parentID; }
		const I4Matrix4x4&		getParentTM() const			{ return parentElement ? parentElement->getResultTM() : I4MATRIX4X4_IDENTITY; }

	protected:
		I4Matrix4x4				resultTM;

		I4Actor*				actor;
		I4ActorElementInfo*		elementInfo;
		I4ActorElement*			parentElement;
		I4AnimationController*	aniController;
	};
	

	//------------------------- I4ActorBone ---------------------

	class I4ActorBone : public I4ActorElement
	{
	public:
		I4ActorBone(I4Actor* actor, I4ActorElementInfo* info);
		virtual ~I4ActorBone();

		virtual bool	initialize() override;
		virtual void	render(I4DeferredRenderer* renderer, const I4Matrix4x4& parentTM) override;

		I4Matrix4x4		getSkinTM() const	{ return worldInverseTM*resultTM; }

	private:
		I4Matrix4x4		worldInverseTM;		// 스킨행렬을 계산하기 위해 미리 역행렬을 구해둔다
	};
	

	//------------------------- I4ActorMesh -------------------------

	class I4ActorMesh : public I4ActorElement
	{
	public:
		I4ActorMesh(I4Actor* actor, I4ActorElementInfo* info, I4Mesh* mesh);
		virtual ~I4ActorMesh();
	
	protected:
		I4Mesh*			mesh;
	};


	//------------------------- ActorRigidMesh -------------------------

	class ActorRigidMesh : public I4ActorMesh
	{
	public:
		ActorRigidMesh(I4Actor* actor, I4ActorElementInfo* info, I4Mesh* mesh);
		virtual ~ActorRigidMesh();

		virtual void	render(I4DeferredRenderer* renderer, const I4Matrix4x4& parentTM) override;
	};

	//------------------------- ActorSkinedMeshGPU -------------------------

	class ActorSkinedMeshGPU : public I4ActorMesh
	{
	public:
		ActorSkinedMeshGPU(I4Actor* actor, I4ActorElementInfo* info, I4Mesh* mesh);
		virtual ~ActorSkinedMeshGPU();

		virtual bool	initialize() override;
		virtual void	animate(float deltaSec, const I4Matrix4x4& parentTM) override;

		virtual void	render(I4DeferredRenderer* renderer, const I4Matrix4x4& parentTM) override;

	private:
		vector<I4Matrix4x4>		matrixPalette;
	};

}
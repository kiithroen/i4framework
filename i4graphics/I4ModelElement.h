#pragma once

#include "I4TriangleMesh.h"
#include "I4ModelElementInfo.h"

namespace i4graphics {

	struct I4KeyFrameSet;
	struct I4Material;
	class I4ShaderMgr;
	class I4Model;
	class I4ModelBoneResource;
	class I4AnimationController;
	class I4Renderer;
	
	//------------------------- I4ModelElement ---------------------

	class I4ModelElement
	{
	public:
		I4ModelElement(I4Model* model, I4ModelElementInfo* info);
		virtual ~I4ModelElement();

		void					registerAni(const char* name, I4KeyFrameSet* keyFrameSet);
		void					playAni(const char* name);

		virtual bool			initialize();
		virtual void			animate(float dt, const I4Matrix4x4& parentTM);
		virtual void			commitToRenderer(I4Renderer* renderer, const I4Matrix4x4& parentTM)	{}

		const I4Matrix4x4& 		getResultTM() const			{ return resultTM;	}

		int						getParentID() const			{ return elementInfo->parentID; }
		const I4Matrix4x4&		getParentTM() const			{ return parentElement ? parentElement->getResultTM() : I4MATRIX4X4_IDENTITY; }

		const char*				getName() const				{ return elementInfo->name.c_str(); }

	protected:
		I4Matrix4x4				resultTM;

		I4Model*				model;
		I4ModelElementInfo*		elementInfo;
		I4ModelElement*			parentElement;
		I4AnimationController*	aniController;
	};
	

	//------------------------- I4ModelBone ---------------------

	class I4ModelBone : public I4ModelElement
	{
	public:
		I4ModelBone(I4Model* model, I4ModelElementInfo* info);
		virtual ~I4ModelBone();

		virtual bool	initialize() override;
		virtual void	commitToRenderer(I4Renderer* renderer, const I4Matrix4x4& parentTM) override;

		I4Matrix4x4		getSkinTM() const	{ return worldInverseTM*resultTM; }

	private:
		I4Matrix4x4		worldInverseTM;		// 스킨행렬을 계산하기 위해 미리 역행렬을 구해둔다
	};
	

	//------------------------- I4ModelMesh -------------------------

	class I4ModelMesh : public I4ModelElement
	{
	public:
		I4ModelMesh(I4Model* model, I4ModelElementInfo* info, I4TriangleMesh* mesh);
		virtual ~I4ModelMesh();
	
		void			setMaterial(I4Material*	mtrl)		{ material = mtrl;}

	protected:
		I4TriangleMesh*			mesh;
		I4Material*		material;
	};


	//------------------------- ModelRigidMesh -------------------------

	class ModelRigidMesh : public I4ModelMesh
	{
	public:
		ModelRigidMesh(I4Model* model, I4ModelElementInfo* info, I4TriangleMesh* mesh);
		virtual ~ModelRigidMesh();

		virtual void	commitToRenderer(I4Renderer* renderer, const I4Matrix4x4& parentTM) override;
	};

	//------------------------- ModelSkinedMeshGPU -------------------------

	class ModelSkinedMeshGPU : public I4ModelMesh
	{
	public:
		ModelSkinedMeshGPU(I4Model* model, I4ModelElementInfo* info, I4TriangleMesh* mesh);
		virtual ~ModelSkinedMeshGPU();

		virtual bool	initialize() override;
		virtual void	animate(float dt, const I4Matrix4x4& parentTM) override;

		virtual void	commitToRenderer(I4Renderer* renderer, const I4Matrix4x4& parentTM) override;

	private:
		vector<I4Matrix4x4>		matrixPalette;
	};

}
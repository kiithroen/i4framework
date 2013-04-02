#pragma once

#include "I4TriangleMesh.h"
#include "I4ModelElementInfo.h"

namespace i4graphics {

	struct KeyFrameSet;
	struct Material;
	class ShaderMgr;
	class Model;
	class ModelBoneResource;
	class AnimationController;
	class Renderer;
	
	//------------------------- ModelElement ---------------------

	class ModelElement
	{
	public:
		ModelElement(Model* model, ModelElementInfo* info);
		virtual ~ModelElement();

		void					registerAni(const char* name, KeyFrameSet* keyFrameSet);
		void					playAni(const char* name);

		virtual bool			initialize();
		virtual void			animate(float dt, const Matrix4x4& parentTM);
		virtual void			commitToRenderer(Renderer* renderer, const Matrix4x4& worldTM)	{}

		const Matrix4x4& 		getResultTM() const			{ return resultTM;	}

		int						getParentID() const			{ return elementInfo->parentID; }
		const Matrix4x4&		getParentTM() const			{ return parentElement ? parentElement->getResultTM() : MATRIX4X4_IDENTITY; }

		const char*				getName() const				{ return elementInfo->name.c_str(); }

	protected:
		Matrix4x4				resultTM;

		Model*				model;
		ModelElementInfo*		elementInfo;
		ModelElement*			parentElement;
		AnimationController*	aniController;
	};
	

	//------------------------- ModelBone ---------------------

	class ModelBone : public ModelElement
	{
	public:
		ModelBone(Model* model, ModelElementInfo* info);
		virtual ~ModelBone();

		virtual bool	initialize() override;
		virtual void	commitToRenderer(Renderer* renderer, const Matrix4x4& parentTM) override;

		Matrix4x4		getSkinTM() const	{ return worldInverseTM*resultTM; }

	private:
		Matrix4x4		worldInverseTM;		// 스킨행렬을 계산하기 위해 미리 역행렬을 구해둔다
	};
	

	//------------------------- ModelMesh -------------------------

	class ModelMesh : public ModelElement
	{
		typedef vector<Material*>		MaterialVector;
	public:
		ModelMesh(Model* model, ModelElementInfo* info, TriangleMesh* mesh);
		virtual ~ModelMesh();
	
		void			setMaterial(unsigned int i, Material*	mtrl)		{ vecMaterial[i] = mtrl;}
		virtual void	commitToRenderer(Renderer* renderer, const Matrix4x4& parentTM) override;

		TriangleMesh*	getMesh()			{ return mesh; }

	protected:
		TriangleMesh*			mesh;
		MaterialVector		vecMaterial;
	};
}
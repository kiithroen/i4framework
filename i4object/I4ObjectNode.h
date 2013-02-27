#pragma once

#include "I4Matrix4x4.h"
#include "I4AABB.h"
#include "I4Quaternion.h"
using namespace i4core;

namespace i4object {

	class ObjectMgr;

	class I4OBJECT_API I4ObjectNode
	{
		typedef vector<I4ObjectNode*>		ObjectNodeVector;
	public:
		I4ObjectNode(I4ObjectNode* parent, const char* name);
		~I4ObjectNode();
				
		I4ObjectNode*			createChild(const char* name);

		void					addChild(I4ObjectNode* child);

		void					removeChild(I4ObjectNode* child);
		void					removeFromParent();
		
		void					destroyChild(I4ObjectNode* child);
		void					destroyChildAll();
		void					destroyFromObject();
		
		void 					setLocalLookAt(const I4Vector3& eye, const I4Vector3& at, const I4Vector3& up);
		void 					setLocalTM(const I4Matrix4x4& localTM);
		void					setLocalRotation(const I4Quaternion& rotation);
		void 					setLocalRotationYawPitchRoll(float yaw, float pitch, float roll);
		void 					setLocalPosition(const I4Vector3& t);
		void 					setLocalScale(const I4Vector3& s);

		const string&			getName()							{ return name; }
		
		const I4Matrix4x4&		getLocalTM() const					{ return localTM; }
		const I4Quaternion&		getLocalRotation() const			{ return localRotation; }
		const I4Vector3&		getLocalPosition() const			{ return localPosition; }
		const I4Vector3&		getLocalScale() const				{ return localScale; }
		const I4Matrix4x4&		getWorldTM() const					{ return worldTM; }

	private:
		void					calcTM();
		void					calcLocalTM();
		void					calcWorldTM();

	protected:
		I4Matrix4x4				localTM;
		I4Matrix4x4				worldTM;
		I4Quaternion			localRotation;
		I4Vector3				localScale;
		I4Vector3				localPosition;

		I4ObjectNode*			parent;
		ObjectNodeVector		vecChild;

		string					name;
	};

}
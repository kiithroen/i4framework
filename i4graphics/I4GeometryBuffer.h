#pragma once

#include "i4graphics.h"
#include "I4Vector2.h"
#include "I4Vector3.h"
#include "I4Vector4.h"
#include "I4Format.h"
using namespace i4core;

namespace i4graphics
{
	enum I4PrimitiveType
	{
		I4PT_POINTLIST		= 0,
		I4PT_LINELIST,
		I4PT_LINESTRIP ,
		I4PT_TRIANGLELIST,
		I4PT_TRIANGLESTRIP,
	};

	enum I4INPUT_CLASSIFICATION
    {
		I4INPUT_PER_VERTEX_DATA	= 0,
		I4INPUT_PER_INSTANCE_DATA	= 1
    };


	struct I4INPUT_ELEMENT
    {
		const char*				SemanticName;
		unsigned int			SemanticIndex;
		I4FORMAT				Format;
		unsigned int			InputSlot;
		unsigned int			AlignedByteOffset;
		I4INPUT_CLASSIFICATION	InputSlotClass;
		unsigned int			InstanceDataStepRate;
    };

//-------------------------------------------------------------------------------------

#define I4TEX_UV_NA 9999.0f
	
	struct I4TextureUV
	{
		I4TextureUV()
			: u(I4TEX_UV_NA)
			, v(I4TEX_UV_NA)
		{
		}

		I4TextureUV(float _u, float _v)
			: u(_u)
			, v(_v)
		{
		}

		float u, v;
	};

	struct I4Index16
	{
		I4Index16()
		{
		}

		I4Index16(unsigned short _i1, unsigned short _i2, unsigned short _i3)
		{			
			i[0] = _i1;
			i[1] = _i2;
			i[2] = _i3;
		}

		unsigned short i[3];
	};
	
	struct I4Vertex_Pos
	{
		float x, y, z;
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
	};

	struct I4Vertex_Pos_Col
	{
		I4Vector3		pos;
		unsigned int	color;	
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_COL[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, I4FORMAT_R8G8B8A8_UNORM, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
	};

	struct I4Vertex_Pos_Tex
	{
		I4Vector3	pos;
		I4TextureUV	uv;
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_TEX[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, I4FORMAT_R32G32_FLOAT, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
	};

	struct I4Vertex_Pos_Normal_Tex
	{
		I4Vector3	position;
		I4Vector3	normal;
		I4TextureUV	uv;
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_NORMAL_TEX[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, I4FORMAT_R32G32B32_FLOAT, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, I4FORMAT_R32G32_FLOAT, 0, 24, I4INPUT_PER_VERTEX_DATA, 0 },
	};

	struct I4Vertex_Pos_Normal_Tex_Tan
	{
		I4Vector3	position;
		I4Vector3	normal;
		I4TextureUV	uv;
		I4Vector4	tangent;
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, I4FORMAT_R32G32B32_FLOAT, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, I4FORMAT_R32G32_FLOAT, 0, 24, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, I4FORMAT_R32G32B32A32_FLOAT, 0, 32, I4INPUT_PER_VERTEX_DATA, 0 },
	};

	struct I4Vertex_Pos_Normal_Tex_Tan_BoneID_Weight
	{
		I4Vector3	position;
		I4Vector3	normal;
		I4TextureUV	uv;
		I4Vector4	tangent;
		float		boneID[4];
		float		weight[4];
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN_BONEID_WEIGHT[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, I4FORMAT_R32G32B32_FLOAT, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, I4FORMAT_R32G32_FLOAT, 0, 24, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, I4FORMAT_R32G32B32A32_FLOAT, 0, 32, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEID", 0, I4FORMAT_R32G32B32A32_FLOAT, 0, 48, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, I4FORMAT_R32G32B32A32_FLOAT, 0, 64, I4INPUT_PER_VERTEX_DATA, 0 },
	};

	inline void calculateTangentArray(const std::vector<I4Vector3>& vecPosition, const std::vector<I4Vector3>& vecNormal,
		const std::vector<I4TextureUV>& vecTexUV, const std::vector<I4Index16>& vecIndex, std::vector<I4Vector4>& vecTangent)
	{
		std::vector<I4Vector3> tan1;
		tan1.resize(vecPosition.size());
		memset(&tan1[0], 0, sizeof(I4Vector3)*tan1.size());

		std::vector<I4Vector3> tan2;
		tan2.resize(vecPosition.size());
		memset(&tan2[0], 0, sizeof(I4Vector3)*tan2.size());
		
		for (unsigned int i = 0; i < vecIndex.size(); ++i)
		{
			unsigned short i1 = vecIndex[i].i[0];
			unsigned short i2 = vecIndex[i].i[1];
			unsigned short i3 = vecIndex[i].i[2];

			const I4Vector3& v1 = vecPosition[i1];
			const I4Vector3& v2 = vecPosition[i2];
			const I4Vector3& v3 = vecPosition[i3];

			const I4TextureUV& w1 = vecTexUV[i1];
			const I4TextureUV& w2 = vecTexUV[i2];
			const I4TextureUV& w3 = vecTexUV[i3];

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.u - w1.u;
			float s2 = w3.u - w1.u;
			float t1 = w2.v - w1.v;
			float t2 = w3.v - w1.v;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			I4Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			I4Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;

			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}

		vecTangent.resize(vecPosition.size());
		for (unsigned int i = 0; i < vecPosition.size(); i++)
		{
			const I4Vector3& n = vecNormal[i];
			const I4Vector3& t = tan1[i];

			I4Vector3 v = (t - n * I4Vector3::dotProduct(n, t));			
			v.normalize();
			if (_finite(v.x) == 0 || _finite(v.y) == 0 || _finite(v.z) == 0)
			{
				v.x = 0;
				v.y = 0;
				v.z = 0;
			}

			vecTangent[i].x = v.x;
			vecTangent[i].y = v.y;
			vecTangent[i].z = v.z;
			vecTangent[i].w = (I4Vector3::dotProduct(I4Vector3::crossProduct(n, t), tan2[i]) < 0.0F) ? -1.0F : 1.0F;
		}
	}

//-------------------------------------------------------------------------------------

	class I4VertexBuffer
	{
	public:
		I4VertexBuffer();
		virtual ~I4VertexBuffer();

		virtual bool		create(unsigned int count, unsigned int stride, const void* vertices = 0);

		virtual bool		copyFrom(const void* data);

		virtual bool		lock(void** data);
		virtual void		unlock();

		virtual void		bind();
		virtual void		unbind();

		virtual void		draw(I4PrimitiveType pt);

		unsigned int		getCount()			{ return count; }
		unsigned int		getStride()			{ return stride; }

	protected:
		unsigned int		count;
		unsigned int		stride;
	};


//-------------------------------------------------------------------------------------

	class I4IndexBuffer
	{
	public:
		I4IndexBuffer();
		virtual ~I4IndexBuffer();

		virtual bool		create(unsigned int count, unsigned int stride, const void* indices = 0);

		virtual bool		copyFrom(const void* data);

		virtual bool		lock(void** data);
		virtual void		unlock();

		virtual void		bind();
		virtual void		unbind();

		virtual void		draw(I4PrimitiveType pt);

		unsigned int		getCount()			{ return count; }
		unsigned int		getStride()			{ return stride; }

	protected:
		unsigned int		count;
		unsigned int		stride;
		I4FORMAT			format;
	};

}
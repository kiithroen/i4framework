#pragma once

#include "I4Vector2.h"
#include "I4Vector3.h"
#include "I4Vector4.h"
#include "I4Format.h"

using namespace i4core;

namespace i4graphics
{
	enum PrimitiveType
	{
		PT_POINTLIST		= 0,
		PT_LINELIST,
		PT_LINESTRIP ,
		PT_TRIANGLELIST,
		PT_TRIANGLESTRIP,
	};

	enum INPUT_CLASSIFICATION
    {
		INPUT_PER_VERTEX_DATA	= 0,
		INPUT_PER_INSTANCE_DATA	= 1
    };


	struct INPUT_ELEMENT
    {
		const char*				SemanticName;
		unsigned int			SemanticIndex;
		FORMAT				Format;
		unsigned int			InputSlot;
		unsigned int			AlignedByteOffset;
		INPUT_CLASSIFICATION	InputSlotClass;
		unsigned int			InstanceDataStepRate;
    };

//-------------------------------------------------------------------------------------

#pragma pack(push, 1)

#define TEX_UV_NA 9999.0f
	
	struct TextureUV
	{
		TextureUV()
			: u(TEX_UV_NA)
			, v(TEX_UV_NA)
		{
		}

		TextureUV(float _u, float _v)
			: u(_u)
			, v(_v)
		{
		}

		float u, v;
	};

	struct Index16
	{
		Index16()
		{
		}

		Index16(unsigned short _i1, unsigned short _i2, unsigned short _i3)
		{			
			i[0] = _i1;
			i[1] = _i2;
			i[2] = _i3;
		}

		unsigned short i[3];
	};
	
	struct BoneID
	{
		unsigned int boneID[4];
	};

	struct Weight
	{
		float weight[4];
	};

	struct Vertex_Pos
	{
		float x, y, z;
	};

	static INPUT_ELEMENT INPUT_ELEMENTS_POS[] = 
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA, 0 },
	};

	struct Vertex_Pos_Col
	{
		Vector3		pos;
		Vector4		color;	
	};

	static INPUT_ELEMENT INPUT_ELEMENTS_POS_COL[] = 
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, FORMAT_R32G32B32A32_FLOAT, 0, 12, INPUT_PER_VERTEX_DATA, 0 },
	};

	struct Vertex_Pos_Tex
	{
		Vector3	pos;
		TextureUV	uv;
	};

	static INPUT_ELEMENT INPUT_ELEMENTS_POS_TEX[] = 
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 12, INPUT_PER_VERTEX_DATA, 0 },
	};

	struct Vertex_Pos_Normal
	{
		Vector3	position;
		Vector3	normal;
	};

	static INPUT_ELEMENT INPUT_ELEMENTS_POS_NORMAL[] = 
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 12, INPUT_PER_VERTEX_DATA, 0 },
	};

	struct Vertex_Pos_Normal_Tex
	{
		Vector3	position;
		Vector3	normal;
		TextureUV	uv;
	};

	static INPUT_ELEMENT INPUT_ELEMENTS_POS_NORMAL_TEX[] = 
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 12, INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 24, INPUT_PER_VERTEX_DATA, 0 },
	};

	struct Vertex_Pos_Normal_Tex_Tan
	{
		Vector3	position;
		Vector3	normal;
		TextureUV	uv;
		Vector4	tangent;
	};

	static INPUT_ELEMENT INPUT_ELEMENTS_POS_NORMAL_TEX_TAN[] = 
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 12, INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 24, INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, FORMAT_R32G32B32A32_FLOAT, 0, 32, INPUT_PER_VERTEX_DATA, 0 },
	};

	struct Vertex_Pos_Normal_Tex_Tan_SkinInfo
	{
		Vector3		position;
		Vector3		normal;
		TextureUV		uv;
		Vector4		tangent;
		BoneID		boneID;
		Weight		weight;
	};

	static INPUT_ELEMENT INPUT_ELEMENTS_POS_NORMAL_TEX_TAN_SKININFO[] = 
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 12, INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 24, INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, FORMAT_R32G32B32A32_FLOAT, 0, 32, INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEID", 0, FORMAT_R32G32B32A32_UINT, 0,     48, INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, FORMAT_R32G32B32A32_FLOAT, 0,  64, INPUT_PER_VERTEX_DATA, 0 },
	};

#pragma pack(pop)

//-------------------------------------------------------------------------------------

	class VertexBuffer
	{
	public:
		VertexBuffer();
		virtual ~VertexBuffer();

		virtual bool		create(unsigned int count, unsigned int stride, const void* vertices = 0);

		virtual bool		copyFrom(const void* data);
		virtual bool		copyFrom(const void* data, unsigned int count);

		virtual bool		lock(void** data);
		virtual void		unlock();

		virtual void		bind();
		virtual void		unbind();

		virtual void		draw(PrimitiveType pt);
		virtual void		draw(PrimitiveType pt, unsigned int count, unsigned int start);

		unsigned int		getCount()			{ return count; }
		unsigned int		getStride()			{ return stride; }

	protected:
		unsigned int		count;
		unsigned int		stride;
	};


//-------------------------------------------------------------------------------------

	class IndexBuffer
	{
	public:
		IndexBuffer();
		virtual ~IndexBuffer();

		virtual bool		create(unsigned int count, unsigned int stride, const void* indices = 0);

		virtual bool		copyFrom(const void* data);
		virtual bool		copyFrom(const void* data, unsigned int count);

		virtual bool		lock(void** data);
		virtual void		unlock();

		virtual void		bind();
		virtual void		unbind();

		virtual void		draw(PrimitiveType pt);
		virtual void		draw(PrimitiveType pt, unsigned int _count, unsigned int _startIndex, int _baseVertex);

		unsigned int		getCount()			{ return count; }
		unsigned int		getStride()			{ return stride; }

	protected:
		unsigned int		count;
		unsigned int		stride;
		FORMAT			format;
	};

	class ConstantBuffer
	{
	public:
		ConstantBuffer();
		virtual ~ConstantBuffer();

		virtual bool		create(unsigned int stride);
	};

}
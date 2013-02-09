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

#pragma pack(push, 1)

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
	
	struct I4SkinInfo
	{

		float	boneID[4];
		float	weight[4];
	};

	struct I4BoneID
	{
		unsigned int boneID[4];
	};

	struct I4Weight
	{
		float weight[4];
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

	struct I4Vertex_Pos_Normal_Tex_Tan_SkinInfo
	{
		I4Vector3		position;
		I4Vector3		normal;
		I4TextureUV		uv;
		I4Vector4		tangent;
		I4BoneID		boneID;
		I4Weight		weight;
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN_SKININFO[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, I4FORMAT_R32G32B32_FLOAT, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, I4FORMAT_R32G32_FLOAT, 0, 24, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, I4FORMAT_R32G32B32A32_FLOAT, 0, 32, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEID", 0, I4FORMAT_R32G32B32A32_UINT, 0,     48, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, I4FORMAT_R32G32B32A32_FLOAT, 0,  64, I4INPUT_PER_VERTEX_DATA, 0 },
	};

#pragma pack(pop)

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

	class I4ConstantBuffer
	{
	public:
		I4ConstantBuffer();
		virtual ~I4ConstantBuffer();

		virtual bool		create(unsigned int stride);
	};

}
#pragma once

#include "i4graphics.h"
#include "I4Vector3.h"
#include "I4Vector4.h"

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

	typedef enum I4FORMAT
	{
		I4FORMAT_UNKNOWN	                 = 0,
		I4FORMAT_R32G32B32A32_TYPELESS       = 1,
		I4FORMAT_R32G32B32A32_FLOAT          = 2,
		I4FORMAT_R32G32B32A32_UINT           = 3,
		I4FORMAT_R32G32B32A32_SINT           = 4,
		I4FORMAT_R32G32B32_TYPELESS          = 5,
		I4FORMAT_R32G32B32_FLOAT             = 6,
		I4FORMAT_R32G32B32_UINT              = 7,
		I4FORMAT_R32G32B32_SINT              = 8,
		I4FORMAT_R16G16B16A16_TYPELESS       = 9,
		I4FORMAT_R16G16B16A16_FLOAT          = 10,
		I4FORMAT_R16G16B16A16_UNORM          = 11,
		I4FORMAT_R16G16B16A16_UINT           = 12,
		I4FORMAT_R16G16B16A16_SNORM          = 13,
		I4FORMAT_R16G16B16A16_SINT           = 14,
		I4FORMAT_R32G32_TYPELESS             = 15,
		I4FORMAT_R32G32_FLOAT                = 16,
		I4FORMAT_R32G32_UINT                 = 17,
		I4FORMAT_R32G32_SINT                 = 18,
		I4FORMAT_R32G8X24_TYPELESS           = 19,
		I4FORMAT_D32_FLOAT_S8X24_UINT        = 20,
		I4FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
		I4FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
		I4FORMAT_R10G10B10A2_TYPELESS        = 23,
		I4FORMAT_R10G10B10A2_UNORM           = 24,
		I4FORMAT_R10G10B10A2_UINT            = 25,
		I4FORMAT_R11G11B10_FLOAT             = 26,
		I4FORMAT_R8G8B8A8_TYPELESS           = 27,
		I4FORMAT_R8G8B8A8_UNORM              = 28,
		I4FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
		I4FORMAT_R8G8B8A8_UINT               = 30,
		I4FORMAT_R8G8B8A8_SNORM              = 31,
		I4FORMAT_R8G8B8A8_SINT               = 32,
		I4FORMAT_R16G16_TYPELESS             = 33,
		I4FORMAT_R16G16_FLOAT                = 34,
		I4FORMAT_R16G16_UNORM                = 35,
		I4FORMAT_R16G16_UINT                 = 36,
		I4FORMAT_R16G16_SNORM                = 37,
		I4FORMAT_R16G16_SINT                 = 38,
		I4FORMAT_R32_TYPELESS                = 39,
		I4FORMAT_D32_FLOAT                   = 40,
		I4FORMAT_R32_FLOAT                   = 41,
		I4FORMAT_R32_UINT                    = 42,
		I4FORMAT_R32_SINT                    = 43,
		I4FORMAT_R24G8_TYPELESS              = 44,
		I4FORMAT_D24_UNORM_S8_UINT           = 45,
		I4FORMAT_R24_UNORM_X8_TYPELESS       = 46,
		I4FORMAT_X24_TYPELESS_G8_UINT        = 47,
		I4FORMAT_R8G8_TYPELESS               = 48,
		I4FORMAT_R8G8_UNORM                  = 49,
		I4FORMAT_R8G8_UINT                   = 50,
		I4FORMAT_R8G8_SNORM                  = 51,
		I4FORMAT_R8G8_SINT                   = 52,
		I4FORMAT_R16_TYPELESS                = 53,
		I4FORMAT_R16_FLOAT                   = 54,
		I4FORMAT_D16_UNORM                   = 55,
		I4FORMAT_R16_UNORM                   = 56,
		I4FORMAT_R16_UINT                    = 57,
		I4FORMAT_R16_SNORM                   = 58,
		I4FORMAT_R16_SINT                    = 59,
		I4FORMAT_R8_TYPELESS                 = 60,
		I4FORMAT_R8_UNORM                    = 61,
		I4FORMAT_R8_UINT                     = 62,
		I4FORMAT_R8_SNORM                    = 63,
		I4FORMAT_R8_SINT                     = 64,
		I4FORMAT_A8_UNORM                    = 65,
		I4FORMAT_R1_UNORM                    = 66,
		I4FORMAT_R9G9B9E5_SHAREDEXP          = 67,
		I4FORMAT_R8G8_B8G8_UNORM             = 68,
		I4FORMAT_G8R8_G8B8_UNORM             = 69,
		I4FORMAT_BC1_TYPELESS                = 70,
		I4FORMAT_BC1_UNORM                   = 71,
		I4FORMAT_BC1_UNORM_SRGB              = 72,
		I4FORMAT_BC2_TYPELESS                = 73,
		I4FORMAT_BC2_UNORM                   = 74,
		I4FORMAT_BC2_UNORM_SRGB              = 75,
		I4FORMAT_BC3_TYPELESS                = 76,
		I4FORMAT_BC3_UNORM                   = 77,
		I4FORMAT_BC3_UNORM_SRGB              = 78,
		I4FORMAT_BC4_TYPELESS                = 79,
		I4FORMAT_BC4_UNORM                   = 80,
		I4FORMAT_BC4_SNORM                   = 81,
		I4FORMAT_BC5_TYPELESS                = 82,
		I4FORMAT_BC5_UNORM                   = 83,
		I4FORMAT_BC5_SNORM                   = 84,
		I4FORMAT_B5G6R5_UNORM                = 85,
		I4FORMAT_B5G5R5A1_UNORM              = 86,
		I4FORMAT_B8G8R8A8_UNORM              = 87,
		I4FORMAT_B8G8R8X8_UNORM              = 88,
		I4FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
		I4FORMAT_B8G8R8A8_TYPELESS           = 90,
		I4FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
		I4FORMAT_B8G8R8X8_TYPELESS           = 92,
		I4FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
		I4FORMAT_BC6H_TYPELESS               = 94,
		I4FORMAT_BC6H_UF16                   = 95,
		I4FORMAT_BC6H_SF16                   = 96,
		I4FORMAT_BC7_TYPELESS                = 97,
		I4FORMAT_BC7_UNORM                   = 98,
		I4FORMAT_BC7_UNORM_SRGB              = 99,
		I4FORMAT_FORCE_UINT                  = 0xffffffff
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
		float u, v;

		I4TextureUV() : u(I4TEX_UV_NA), v(I4TEX_UV_NA) {}
	};

	struct I4Index16
	{
		unsigned short i[3];
	};

	struct I4SkinInfo
	{

		float	boneID[4];
		float	weight[4];
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
		float x, y, z;
		unsigned int color;	
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_COL[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, I4FORMAT_R8G8B8A8_UNORM, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
	};

	struct I4Vertex_Pos_Tex
	{
		float x, y, z;
		float u, v;
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_TEX[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD0", 0, I4FORMAT_R32G32_FLOAT, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
	};

	struct I4Vertex_Pos_Normal_Tan_Tex 
	{
		I4Vector3	position;
		I4Vector3	normal;
		I4TextureUV	uv;
		I4Vector4	tangent;
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_NORMAL_TAN_TEX[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, I4FORMAT_R32G32B32_FLOAT, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD0", 0, I4FORMAT_R32G32_FLOAT, 0, 24, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, I4FORMAT_R32G32_FLOAT, 0, 32, I4INPUT_PER_VERTEX_DATA, 0 },
	};

	struct I4Vertex_Pos_Normal_Tan_Tex_I4SkinInfo
	{
		I4Vector3	position;
		I4Vector3	normal;
		I4TextureUV	texUV;
		I4SkinInfo	skinInfo;
		I4Vector4	tangent;
	};

	static I4INPUT_ELEMENT I4INPUT_ELEMENTS_POS_NORMAL_TAN_TEX_SKININFO[] = 
	{
		{ "POSITION", 0, I4FORMAT_R32G32B32_FLOAT, 0, 0, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, I4FORMAT_R32G32B32_FLOAT, 0, 12, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD0", 0, I4FORMAT_R32G32_FLOAT, 0, 24, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEID", 0, I4FORMAT_R32G32B32A32_FLOAT, 0, 32, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, I4FORMAT_R32G32_FLOAT, 0, 48, I4INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, I4FORMAT_R32G32_FLOAT, 0, 64, I4INPUT_PER_VERTEX_DATA, 0 },
	};


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

		virtual void		draw(I4PrimitiveType pt, unsigned int verticeCount);

		unsigned int		getCount()			{ return count; }
		unsigned int		getStride()			{ return stride; }

	protected:
		unsigned int		count;
		unsigned int		stride;
	};

}
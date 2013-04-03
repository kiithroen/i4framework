
cbuffer cbPerObject : register(b0)
{
    matrix        g_mWorldViewProjection ;
};

#ifdef MASK_SKINNING
cbuffer CBEachSkinedMesh_S : register(b1)
{
	matrix matrixPalette[128];
};
#endif

struct VS_INPUT
{
    float3 position    : POSITION;
	float3 normal		:	NORMAL;
	float2 uv			:	TEXCOORD;
	float4 tangent		:	TANGENT;

#ifdef MASK_SKINNING
	uint4 boneID		:	BONEID;
	float4 weight		:	WEIGHT;
#endif
};

struct VS_OUTPUT
{
    float4 position    : SV_POSITION;
};

VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output;
    
#ifdef MASK_SKINNING
	float4 position = mul(float4(input.position, 1), matrixPalette[input.boneID.x])*input.weight.x;
	position += mul(float4(input.position, 1), matrixPalette[input.boneID.y])*input.weight.y;
	position += mul(float4(input.position, 1), matrixPalette[input.boneID.z])*input.weight.z;
	position += mul(float4(input.position, 1), matrixPalette[input.boneID.w])*input.weight.w;
#else
    float4 position = float4(input.position, 1);
#endif

	output.position = mul(position, g_mWorldViewProjection);

    return output;
}

void PS(VS_OUTPUT input)
{
}
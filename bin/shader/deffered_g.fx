cbuffer	CBOnResize_G : register(b0)
{
	matrix projection;
	float farDistance;
};

cbuffer	CBEveryFrame_G : register(b1)
{
	matrix view;
};

cbuffer	CBEachMeshInstance_G : register(b2)
{
	matrix world;
	float specularIntensity;
	float specularPower;
};

#ifdef MASK_SKINNING
cbuffer CBEachAnimation_G : register(b3)
{
	matrix matrixPalette[80];
};
#endif

#ifdef MASK_TEX_DIFFUSE
Texture2D texDiffuseMap : register(t0);
#endif

#ifdef MASK_TEX_SPECULAR
Texture2D texSpecularMap : register(t1);
#endif

#ifdef MASK_TEX_NORMAL
Texture2D texNormalMap : register(t2);
#endif



SamplerState samLinear : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_INPUT
{
	float3 position		:	POSITION;
	float3 normal		:	NORMAL;
	float2 uv			:	TEXCOORD;
	float4 tangent		:	TANGENT;
	
#ifdef MASK_SKINNING
	uint4 boneID		:	BONEID;
	float4 weight		:	WEIGHT;
#endif
};

struct PS_INPUT
{
	float4 position	 	:	SV_POSITION;
	float2 uv			:	TEXCOORD0;
	float depth			:	TEXCOORD1;

#ifdef MASK_TEX_NORMAL
	float3x3 tangentToView	: TEXCOORD2;
#else
	float3 viewNormal	:	TEXCOORD2;
#endif
};

PS_INPUT VS( VS_INPUT	input	)
{
	PS_INPUT output	=	(PS_INPUT)0;


#ifdef MASK_SKINNING
	float3 position = mul(float4(input.position, 1), matrixPalette[input.boneID.x])*input.weight.x;
	position += mul(float4(input.position, 1), matrixPalette[input.boneID.y])*input.weight.y;
	position += mul(float4(input.position, 1), matrixPalette[input.boneID.z])*input.weight.z;
	position += mul(float4(input.position, 1), matrixPalette[input.boneID.w])*input.weight.w;

	float3 normal = mul(input.normal, (float3x3)matrixPalette[input.boneID.x])*input.weight.x;
	normal += mul(input.normal, (float3x3)matrixPalette[input.boneID.y])*input.weight.y;
	normal += mul(input.normal, (float3x3)matrixPalette[input.boneID.z])*input.weight.z;
	normal += mul(input.normal, (float3x3)matrixPalette[input.boneID.w])*input.weight.w;
		
	float3 tangent = mul(input.tangent.xyz, (float3x3)matrixPalette[input.boneID.x])*input.weight.x;
	tangent += mul(input.tangent.xyz, (float3x3)matrixPalette[input.boneID.y])*input.weight.y;
	tangent += mul(input.tangent.xyz, (float3x3)matrixPalette[input.boneID.z])*input.weight.z;
	tangent += mul(input.tangent.xyz, (float3x3)matrixPalette[input.boneID.w])*input.weight.w;

#else
	float3 position = input.position;
	float3 normal = input.normal;
	float3 tangent = input.tangent.xyz;
#endif

	output.position = mul(float4(position,1), world);
	output.position = mul(output.position, view);

	output.depth = output.position.z/farDistance;

	output.position = mul(output.position, projection);

	output.uv = input.uv;

#ifdef MASK_TEX_NORMAL
	float3 N = mul(input.normal, (float3x3)world);
	N = mul(N, (float3x3)view);
	N = normalize(N);

	float3 T = mul(input.tangent.xyz, (float3x3)world);
	T = mul(T, (float3x3)view);
	T = normalize(T);

	float3 B = normalize(cross(N, T)*input.tangent.w);

	output.tangentToView[0] = T;
	output.tangentToView[1] = B;
	output.tangentToView[2] = N;
#else
	output.viewNormal = mul(input.normal, (float3x3)world);
	output.viewNormal = mul(output.viewNormal, (float3x3)view);
#endif
	return output;
}

struct PS_OUTPUT
{
	float4 diffuse : SV_Target0;
	float4 normal : SV_Target1;
	float4 depth : SV_Target2;
};

PS_OUTPUT PS( PS_INPUT	input	)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

#ifdef MASK_TEX_DIFFUSE
	output.diffuse.rgb = texDiffuseMap.Sample(samLinear, input.uv).rgb;
#else
	output.diffuse.rgb = float3(1.0f, 1.0f, 1.0f);
#endif

#ifdef MASK_TEX_SPECULAR
	output.diffuse.a = texSpecularMap.Sample(samLinear, input.uv).r*specularIntensity;
#else
	output.diffuse.a = specularIntensity;
#endif

	float3 viewNormal;
#ifdef MASK_TEX_NORMAL
	viewNormal = 2.0f*texNormalMap.Sample(samLinear, input.uv) - 1.0f;
	viewNormal = mul(viewNormal, input.tangentToView);
#else
	viewNormal = input.viewNormal;
#endif
	
	output.normal.rgb = 0.5f*(normalize(viewNormal) + 1.0f);
	output.normal.a = specularPower;

	output.depth = input.depth;

	return output;
}
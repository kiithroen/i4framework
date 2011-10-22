cbuffer	cbChageOnResize
{
	matrix projection		: PROJECTION;
	float farDistance		: FAR_DISTANCE;
};

cbuffer	cbChangesEveryFrame
{
	matrix world			: WORLD;
	matrix view				: VIEW;
};

#ifdef MASK_DIFFUSEMAP
Texture2D texDiffuseMap		: DIFFUSEMAP;
#endif

#ifdef MASK_SPECULARMAP
Texture2D texSpecularMap	: SPECULARMAP;
#endif

float specularIntensity 	: SPECULAR_INTENSITY;
float specularPower 		: SPECULAR_POWER;

#ifdef MASK_NORMALMAP
Texture2D texNormalMap		: NORMALMAP;
#endif



SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

BlendState BlendingNone
{
	BlendEnable[0] = FALSE;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 pos		:	POSITION;
	float3 normal	:	NORMAL;
	float2 uv		:	TEXCOORD0;

#ifdef MASK_NORMALMAP
	float4 tan		:	TANGENT;
#endif
};

struct PS_INPUT
{
	float4 pos	 		:	SV_POSITION;
	float2 uv			:	TEXCOORD0;
	float depth			:	TEXCOORD1;

#ifdef MASK_NORMALMAP
	float3x3 tangentToView	: TEXCOORD2;
#else
	float3 viewNormal	:	TEXCOORD2;
#endif
};

PS_INPUT VS( VS_INPUT	input	)
{
	PS_INPUT output	=	(PS_INPUT)0;

	output.pos = mul(float4(input.pos,1), world);
	output.pos = mul(output.pos, view);

	output.depth = output.pos.z/farDistance;

	output.pos = mul(output.pos, projection);

	output.uv = input.uv;

#ifdef MASK_NORMALMAP
	float3 N = mul(input.normal, (float3x3)world);
	N = mul(N, (float3x3)view);
	N = normalize(N);

	float3 T = mul(input.tan.xyz, (float3x3)world);
	T = mul(T, (float3x3)view);
	T = normalize(T);

	float3 B = normalize(cross(N, T)*input.tan.w);

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

#ifdef MASK_DIFFUSEMAP
	output.diffuse.rgb = texDiffuseMap.Sample(samLinear, input.uv).rgb;
#else
	output.diffuse.rgb = float3(1.0f, 1.0f, 1.0f);
#endif

#ifdef MASK_SPECULARMAP
	output.diffuse.a = texSpecularMap.Sample(samLinear, input.uv).r*specularIntensity;
#else
	output.diffuse.a = specularIntensity;
#endif

	float3 viewNormal;
#ifdef MASK_NORMALMAP
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

//--------------------------------------------------------------------------------------
technique10	Render
{
	pass P0
	{
		SetVertexShader( CompileShader(	vs_4_0,	VS() ) );
		SetGeometryShader( NULL	);
		SetPixelShader(	CompileShader( ps_4_0, PS()	)	);
		SetBlendState(BlendingNone, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}

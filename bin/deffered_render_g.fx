cbuffer	cbNeverChanges
{
	matrix View		:	VIEW;
};

cbuffer	cbChageOnResize
{
	matrix Projection	:	PROJECTION;
};

cbuffer	cbChangesEveryFrame
{
	matrix World	:	WORLD;
};

Texture2D texDiffuseMap : DIFFUSEMAP;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float specularIntensity = 0.8f;
float specularPower = 0.5f;

//--------------------------------------------------------------------------------------
struct VS_INPUT_G
{
	float3 pos		:	POSITION;
	float3 normal	:	NORMAL;
	float2 tex		:	TEXCOORD0;
};

struct PS_INPUT_G
{
	float4 pos	 	:	SV_POSITION;
	float4 projPos	:	TEXCOORD0;
	float3 normal	:	TEXCOORD1;
	float2 tex		:	TEXCOORD2;
};

PS_INPUT_G VS_G( VS_INPUT_G	input	)
{
	PS_INPUT_G output	=	(PS_INPUT_G)0;

	output.pos = mul(float4(input.pos,	1),	World);
	output.pos = mul(output.pos,	View);
	output.pos = mul(output.pos,	Projection);

	output.projPos = output.pos;

	output.normal = mul(input.normal, (float3x3)World);
	output.normal = mul(output.normal, (float3x3)View);

	output.tex = input.tex;

	return output;
}

struct PS_G_OUTPUT
{
	float4 Diffuse : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Depth : SV_Target2;
};

PS_G_OUTPUT PS_G( PS_INPUT_G	input	)
{
	PS_G_OUTPUT output = (PS_G_OUTPUT)0;
	output.Diffuse = texDiffuseMap.Sample(samLinear, input.tex);
	output.Diffuse.a = specularIntensity;

	output.Normal.rgb = 0.5f*(normalize(input.normal) + 1.0f);
	output.Normal.a = specularPower;

	output.Depth = input.projPos.z/input.projPos.w;

	return output;
}

//--------------------------------------------------------------------------------------
technique10	Render
{
	pass P0
	{
		SetVertexShader( CompileShader(	vs_4_0,	VS_G() ) );
		SetGeometryShader( NULL	);
		SetPixelShader(	CompileShader( ps_4_0, PS_G()	)	);
	}
}

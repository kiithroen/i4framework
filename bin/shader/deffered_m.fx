Texture2D texRTDiffuse : register(t0);
Texture2D texRTLight : register(t1);

SamplerState samLinear : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct VS_INPUT
{
	float3 pos		:	POSITION;
	float2 uv		:	TEXCOORD0;
};

struct PS_INPUT
{
	float4 pos 		:	SV_POSITION;
	float2 uv		:	TEXCOORD0;
};

PS_INPUT VS( VS_INPUT	input	)
{
	PS_INPUT output	=	(PS_INPUT)0;

	output.pos = float4(input.pos,	1);
	output.uv = input.uv;

	return output;
}

float4 PS( PS_INPUT	input	) : SV_Target
{
	float3 diffuseColor = texRTDiffuse.Sample(samLinear, input.uv).rgb;
	float4 light = texRTLight.Sample(samLinear, input.uv);
	float3 diffuseLight = light.rgb;
	float specularLight = light.a;

	float3 finalColor = diffuseColor*diffuseLight;// + specularLight;
	return float4(finalColor, 1);
}
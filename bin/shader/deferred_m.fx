Texture2D texRTDiffuse : register(t0);
Texture2D texRTSpecular : register(t1);
Texture2D texRTLight : register(t2);

SamplerState samLinear : register(s0);

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

	float4 specularData = texRTSpecular.Sample(samLinear, input.uv);
	float3 specularColor = specularData.xyz;

	float4 light = texRTLight.Sample(samLinear, input.uv);
	float3 diffuseLight = light.rgb;
	float specularLight = light.a;

	float emissive = 0.35f;	// 너무 어두워서 이쁘게 보이려고 임시로..
	float3 finalColor = emissive*diffuseColor + diffuseColor*diffuseLight + specularColor*specularLight;

	//finalColor = diffuseLight + specularColor*specularLight;
	return float4(finalColor, 1);
}
Texture2D texRTDiffuse : register(t0);
Texture2D texRTNormal : register(t1);
Texture2D texRTDepth : register(t2);

SamplerState samLinear : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState samPoint : register(s1)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

cbuffer CBOnResize_L_directional : register(b0)
{
	float3 farTopRight	: FAR_TOP_RIGHT;
};

cbuffer CBEachLight_L_directional : register(b1)
{
	float3 lightViewDirection;
	float padding;
	float3 lightColor;
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

	output.pos = float4(input.pos, 1.0f);
	output.uv = input.uv;

	return output;
}

float4 PS( PS_INPUT	input	) : SV_Target
{
	float4 normalData = texRTNormal.Sample(samPoint, input.uv);
	float3 normal = 2.0f * normalData.xyz - 1.0f;
	float specularPower = normalData.a * 255.0f;
	float specularIntensity = texRTDiffuse.Sample(samLinear, input.uv).a;
	float depthVal = texRTDepth.Sample(samPoint, input.uv).r;
	float3 ray;
	ray.x = lerp(-farTopRight.x, farTopRight.x, input.uv.x);
	ray.y = lerp(farTopRight.y, -farTopRight.y, input.uv.y);
	ray.z = farTopRight.z;
	float3 p = ray*depthVal;
	
	float3 lightVector = -normalize(lightViewDirection);

	// ------ lambert -----
	/*
	float NdL = max(0, dot(normal, lightVector));
	float3 diffuseLight = NdL*lightColor.rgb;
	*/

	// ------ half lambert -----
	float NdL = dot(normal, lightVector);
	float3 diffuseLight = saturate(pow(NdL*0.5 + 0.5, 2.0))*lightColor.rgb;

	float3 dirToCamera = -normalize(p);
	float3 reflectVector = normalize(reflect(-lightVector, normal));
	float specularLight = specularIntensity*NdL*pow(saturate(dot(reflectVector, dirToCamera)), specularPower);

	return float4(diffuseLight.rgb, specularLight);
}
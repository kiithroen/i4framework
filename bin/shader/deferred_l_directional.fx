#define SHADOW_SPLIT_NUM 4

Texture2D texRTNormal : register(t0);
Texture2D texRTDepth : register(t1);
Texture2D texRTShadow : register(t2);

SamplerState samLinear : register(s0);

SamplerState samPoint : register(s1);

SamplerComparisonState samShadow : register(s2);

cbuffer CBOnResize_L_directional : register(b0)
{
	matrix viewInvLightViewProjection[4];
	float4 shadowSplitZ;
	float4 shadowBias;
	float3 farTopRight;
	float shadowSplitSize;
	float3 lightViewDirection;
	float padding0;
	float3 lightColor;
	float padding1;
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

	output.pos = float4(input.pos, 1.0);
	output.uv = input.uv;

	return output;
}

float2 texOffset(int u, int v)
{
    return float2(u/(shadowSplitSize*SHADOW_SPLIT_NUM), v/shadowSplitSize);
}

float4 PS( PS_INPUT	input	) : SV_Target
{
	float depthVal = texRTDepth.Sample(samPoint, input.uv).r;
	float3 ray;
	ray.x = lerp(-farTopRight.x, farTopRight.x, input.uv.x);
	ray.y = lerp(farTopRight.y, -farTopRight.y, input.uv.y);
	ray.z = farTopRight.z;
	float3 p = ray*depthVal;

	int shadowSplitIndex = 0;

	for (int i = SHADOW_SPLIT_NUM - 1; i > 0; --i)
	{
		if (p.z > shadowSplitZ[i - 1])
		{
			shadowSplitIndex = i;
			break;
		}
	}
	
	float4 posInLight = mul(float4(p, 1.0f), viewInvLightViewProjection[shadowSplitIndex]);

	float2 shadowUV = 0.5f*(float2(posInLight.x, -posInLight.y)/posInLight.w + 1.0f);

	shadowUV.x = shadowUV.x/(float)SHADOW_SPLIT_NUM + (float)shadowSplitIndex/(float)SHADOW_SPLIT_NUM;

	float depthInLight = posInLight.z/posInLight.w;
	float shadowFactor = 0;
	
	// 4x4 PCF
	for (float y = -1.5; y <= 1.5; y += 1.0)
	{
		for (float x = -1.5; x <= 1.5; x += 1.0)
		{
			shadowFactor += texRTShadow.SampleCmpLevelZero(samShadow, shadowUV + texOffset(x, y), depthInLight - shadowBias[shadowSplitIndex]);
		}
	}
 	
	shadowFactor /= 16.0f;

	float3 diffuseLight = float3(0, 0, 0);
	float specularLight = 0;

	if (shadowFactor > 0.1f)
	{
		float4 normalData = texRTNormal.Sample(samPoint, input.uv);
		float3 normal = 2.0f * normalData.xyz - 1.0f;

		float specularPower = normalData.a * 255.0f;

		float3 lightVector = -normalize(lightViewDirection);
	
		// ------ half lambert -----
		float NdL = dot(normal, lightVector);
		diffuseLight = saturate(pow(NdL*0.5 + 0.5, 2.0))*lightColor.rgb;

		float3 dirToCamera = -normalize(p);
		float3 reflectVector = normalize(reflect(-lightVector, normal));
		specularLight = NdL*pow(saturate(dot(reflectVector, dirToCamera)), specularPower);
	}

	return shadowFactor*float4(diffuseLight.rgb, specularLight);
}
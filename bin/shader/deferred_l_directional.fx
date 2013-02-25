Texture2D texRTDiffuse : register(t0);
Texture2D texRTNormal : register(t1);
Texture2D texRTDepth : register(t2);
Texture2D texRTShadow : register(t3);

SamplerState samLinear : register(s0);

SamplerState samPoint : register(s1);

SamplerComparisonState samShadow : register(s2);

cbuffer CBOnResize_L_directional : register(b0)
{
	float3 farTopRight	: FAR_TOP_RIGHT;
};

cbuffer CBEachLight_L_directional : register(b1)
{
	matrix viewInvLightViewProjection[4];
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

	output.pos = float4(input.pos, 1.0);
	output.uv = input.uv;

	return output;
}

float2 texOffset(int u, int v)
{
    return float2(u *1.0f/4096.0f, v*1.0f/1024.0f);
}

float4 PS( PS_INPUT	input	) : SV_Target
{
	float4 normalData = texRTNormal.Sample(samPoint, input.uv);
	float3 normal = 2.0f * normalData.xyz - 1.0f;

	float specularPower = normalData.a * 255.0f;

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

	// ------ shadow -------

	float index1 = 3.6f;
	float index2 = 7.1f;
	float index3 = 20.0f;

	float3 color;
	int i = 0;
	if (p.z <= index1)
	{
		i = 0;
		color = float3(1, 0, 0);
	}
	else if (p.z <= index2)
	{
		i = 1;
		color = float3(0, 1, 0);
	}
	else if (p.z <= index3)
	{
		i = 2;
		color = float3(0, 0, 1);
	}
	else
	{
		i = 3;
		color = float3(1, 0, 1);
	}

	float shadowSplitLevel = 4;

	float4 posInLight = mul(float4(p, 1.0f), viewInvLightViewProjection[i]);

	float2 shadowUV = 0.5f*(float2(posInLight.x, -posInLight.y)/posInLight.w + 1.0f);

	shadowUV.x = shadowUV.x/shadowSplitLevel + i/shadowSplitLevel;

	float depthInLight = posInLight.z/posInLight.w;
	float shadowFactor = 0;
	
	// 4x4 PCF
	for (float y = -1.5; y <= 1.5; y += 1.0)
	{
		for (float x = -1.5; x <= 1.5; x += 1.0)
		{
			shadowFactor += texRTShadow.SampleCmpLevelZero(samShadow, shadowUV + texOffset(x, y), depthInLight - 0.005f);
		}
	}
 	
	shadowFactor /= 16.0f;

	float3 dirToCamera = -normalize(p);
	float3 reflectVector = normalize(reflect(-lightVector, normal));
	float specularLight = NdL*pow(saturate(dot(reflectVector, dirToCamera)), specularPower);

	return float4(shadowFactor*diffuseLight.rgb, specularLight);
}
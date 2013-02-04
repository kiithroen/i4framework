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

cbuffer cbChangeOnResize_L_point_VS : register(b0)
{
	matrix projection;
}

cbuffer cbChangeOnResize_L_point_PS : register(b1)
{
	float3 farTopRight;
};

cbuffer cbChangeEveryFrame_L_Point : register(b2)
{
	matrix view;
};

cbuffer cbChangeEachLight_L_point_VS : register(b3)
{
	matrix world;
};

cbuffer cbChangeEachLight_L_point_PS : register(b4)
{
	float3	lightPosition;
	float	lightRadius;
	float3	lightColor;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 pos		:	POSITION;
};

struct PS_INPUT
{
	float4 pos 		:	SV_POSITION;
	float4 projPos	:	TEXCOORD0;
};

PS_INPUT VS( VS_INPUT	input	)
{
	PS_INPUT output	=	(PS_INPUT)0;

	output.pos = mul(float4(input.pos, 1), world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	output.projPos = output.pos;

	return output;
}

float4 PS( PS_INPUT	input	) : SV_Target
{
	input.projPos.xy /= input.projPos.w;
	float2 uv = 0.5f*(float2(input.projPos.x, -input.projPos.y) + 1);
	float4 normalData = texRTNormal.Sample(samPoint, uv);
	float3 normal = 2.0f * normalData.xyz - 1.0f;
	float specularPower = normalData.a * 255;
	float specularIntensity = texRTDiffuse.Sample(samLinear, uv).a;
	float depthVal = texRTDepth.Sample(samPoint, uv).r;

	float3 ray;
	ray.x = lerp(-farTopRight.x, farTopRight.x, uv.x);
	ray.y = lerp(farTopRight.y, -farTopRight.y, uv.y);
	ray.z = farTopRight.z;
	float3 p = ray*depthVal;
	float3 lightVector = lightPosition - p;
	float lightDist = length(lightVector);
	float attenuation = saturate(1.0f - lightDist/lightRadius);
	lightVector = normalize(lightVector);
	float NdL = max(0, dot(normal, lightVector));
	float3 diffuseLight = NdL*lightColor.rgb;

	float3 reflectVector = normalize(reflect(-lightVector, normal));
	float3 dirToCamera = -normalize(p);
	float specularLight = specularIntensity*NdL*pow(saturate(dot(reflectVector, dirToCamera)), specularPower);

	return attenuation*float4(diffuseLight.rgb, specularLight);
}

//--------------------------------------------------------------------------------------
technique10	Render
{
	pass P0
	{
		SetVertexShader( CompileShader(	vs_4_0,	VS() ) );
		SetGeometryShader( NULL	);
		SetPixelShader(	CompileShader( ps_4_0, PS()	)	);
	}
}

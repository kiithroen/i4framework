Texture2D texRTDiffuse	: RT_DIFFUSE;
Texture2D texRTNormal	: RT_NORMAL;
Texture2D texRTDepth	: RT_DEPTH;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

BlendState BlendingAdd
{
	BlendEnable[0] = TRUE;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ONE;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

cbuffer cbChangeOnResize
{
	matrix projection		: PROJECTION;
	float3 farTopRight		: FAR_TOP_RIGHT;
};

cbuffer cbChangeEachLight
{
	float4 lightPointRadius	: LIGHT_POINT_RADIUS;
	float3 lightColor 		: LIGHT_COLOR;
};

cbuffer cbChangeEveryFrame
{
	matrix world			: WORLD;
	matrix view				: VIEW;
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
	float2 texCoord = 0.5f*(float2(input.projPos.x, -input.projPos.y) + 1);
	float4 normalData = texRTNormal.Sample(samPoint, texCoord);
	float3 normal = 2.0f * normalData.xyz - 1.0f;
	float specularPower = normalData.a * 255;
	float specularIntensity = texRTDiffuse.Sample(samLinear, texCoord).a;
	float depthVal = texRTDepth.Sample(samPoint, texCoord).r;

	float3 ray;
	ray.x = lerp(-farTopRight.x, farTopRight.x, texCoord.x);
	ray.y = lerp(farTopRight.y, -farTopRight.y, texCoord.y);
	ray.z = farTopRight.z;
	float3 p = ray*depthVal;
	float3 lightVector = lightPointRadius.xyz - p;
	float lightDist = length(lightVector);
	float attenuation = saturate(1.0f - lightDist/lightPointRadius.w);
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
		SetBlendState(BlendingAdd, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}

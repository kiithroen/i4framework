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
	float3 farTopRight	: FAR_TOP_RIGHT;
};

cbuffer cbChangeEachLight
{
	float3 lightDirection	: LIGHT_DIRECTION;
	float3 lightColor		: LIGHT_COLOR;
};

//--------------------------------------------------------------------------------------
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

	float3 lightVector = -normalize(lightDirection);
	float NdL = max(0, dot(normal, lightVector));
	float3 diffuseLight = NdL*lightColor.rgb;

	float3 reflectVector = normalize(reflect(-lightVector, normal));
	float3 dirToCamera = -normalize(p);
	float specularLight = specularIntensity*pow(saturate(dot(reflectVector, dirToCamera)), specularPower);

	return float4(diffuseLight.rgb, specularLight);
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

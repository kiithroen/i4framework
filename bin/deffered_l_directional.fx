Texture2D texRTDiffuse	: RT_DIFFUSE;
Texture2D texRTNormal	: RT_NORMAL;
Texture2D texRTDepth	: RT_DEPTH;
Texture2D texRTShadow	: RT_SHADOW;

float4x4	lightViewProjection	: LIGHT_VIEW_PROJECTION;
float4x4	viewInvLightViewProjection : VIEWINV_LIGHT_VIEW_PROJECTION;

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

	//--------------------
	float4 posInLight = mul(float4(p, 1.0f), viewInvLightViewProjection);
	float depthInLight = posInLight.z/posInLight.w;

	float2 shadowUV = 0.5f*(float2(posInLight.x, -posInLight.y)/posInLight.w + 1.0f);
	float depthInShadow = texRTShadow.Sample(samPoint, shadowUV).r;
	
	float4 shadow = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (depthInLight > depthInShadow + 0.00002f)
		shadow = float4(0.15f, 0.15f, 0.15f, 0.0f);
		
	//--------------------

	float3 lightVector = -normalize(lightDirection);

	// ------ lambert -----
//	float NdL = max(0, dot(normal, lightVector));
//	float3 diffuseLight = NdL*lightColor.rgb;

	// ------ half lambert -----
	float NdL = dot(normal, lightVector);
	float3 diffuseLight = saturate(pow(NdL*0.5 + 0.5, 2.0))*lightColor.rgb;

	float3 dirToCamera = -normalize(p);
	float3 reflectVector = normalize(reflect(-lightVector, normal));
	float specularLight = specularIntensity*NdL*pow(saturate(dot(reflectVector, dirToCamera)), specularPower);

	return shadow*float4(diffuseLight.rgb, specularLight);
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

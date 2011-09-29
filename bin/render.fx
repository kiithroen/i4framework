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

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 pos		:	POSITION;
	float4 color	:	COLOR0;
};

struct PS_INPUT
{
	float4 pos 		:	SV_POSITION;
	float4 color	:	COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex	Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT	input	)
{
	PS_INPUT output	=	(PS_INPUT)0;
	output.pos = mul(float4(input.pos,	1),	World);
	output.pos = mul(output.pos,	View);
	output.pos = mul(output.pos,	Projection);

	output.color = input.color;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT	input	)	:	SV_Target
{
	float4 finalColor	=	input.color;
	return finalColor;
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

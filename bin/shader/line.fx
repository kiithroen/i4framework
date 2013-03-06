cbuffer CBEachFrame_Line : register( b0 )
{
	matrix viewProjection;
}

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;	
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.position = mul(float4(input.position, 1), viewProjection);
    output.color = input.color;

    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
    return input.color;
}

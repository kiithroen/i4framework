
cbuffer cbPerObject : register(b0)
{
    matrix        g_mWorldViewProjection ;
};

struct VS_INPUT
{
    float4 vPosition    : POSITION;
};

struct VS_OUTPUT
{
    float4 vPosition    : SV_POSITION;
};

VS_OUTPUT VS( VS_INPUT Input )
{
    VS_OUTPUT Output;
    
    // There is nothing special here, just transform and write out the depth.
    Output.vPosition = mul( Input.vPosition, g_mWorldViewProjection );

    return Output;
}

void PS(VS_OUTPUT input)
{
}
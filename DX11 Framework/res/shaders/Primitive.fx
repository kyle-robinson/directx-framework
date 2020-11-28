#pragma pack_matrix( row_major )

// vertex shader
cbuffer ConstantBuffer : register( b0 )
{
	float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

struct VS_INPUT
{
    float3 inPos : POSITION;
    float4 inCol : COLOR;
};

struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float4 outCol : COLOR;
};

VS_OUTPUT VS( VS_INPUT input )
{    
    VS_OUTPUT output;
    output.outPos = mul( float4( input.inPos, 1.0f ), worldMatrix );
    output.outPos = mul( output.outPos, viewMatrix );
    output.outPos = mul( output.outPos, projectionMatrix );
    output.outCol = input.inCol;
    return output;
}

// pixel shader
cbuffer ColorBuffer : register( b1 )
{
    float3 outlineColor;
}

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float4 inCol : COLOR;
};

float4 PS( PS_INPUT input ) : SV_TARGET
{
    return float4( outlineColor, 1.0f );
}

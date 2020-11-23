#pragma pack_matrix( row_major )

cbuffer ConstantBuffer : register( b0 )
{
	float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

// vertex shader
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
    //input.inPos.xy += 0.5f * sin( input.inPos.x ) * sin( 3.0f * gTime ); 
    //input.inPos.z *= 0.6f + 0.4f * sin( 2.0f * gTime );
    
    VS_OUTPUT output;
    output.outPos = mul( float4( input.inPos, 1.0f ), worldMatrix );
    output.outPos = mul( output.outPos, viewMatrix );
    output.outPos = mul( output.outPos, projectionMatrix );
    output.outCol = input.inCol;
    return output;
}

// pixel shader
struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float4 inCol : COLOR;
};

float4 PS( PS_INPUT input ) : SV_TARGET
{
    return float4( 1.0f, 0.0f, 0.0f, 1.0f );
}

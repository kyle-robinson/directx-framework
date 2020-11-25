#pragma pack_matrix( row_major )

// vertex shader
cbuffer ConstantBuffer : register( b0 )
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
}

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTex : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float2 outTex : TEXCOORD;
};

VS_OUTPUT VS( VS_INPUT input )
{       
    VS_OUTPUT output;
    output.outPos = mul( float4( input.inPos, 1.0f ), World );
    output.outPos = mul( output.outPos, View );
    output.outPos = mul( output.outPos, Projection );
    output.outPos = output.outPos.xyww;
    output.outTex = input.inTex;
    return output;
}

// pixel shader
struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTex : TEXCOORD;
};

Texture2D skyTexture : TEXTURE : register( t0 );
SamplerState samplerState : SAMPLER : register( s0 );

float4 PS( PS_INPUT input ) : SV_TARGET
{
    float3 pixelColor = skyTexture.Sample( samplerState, input.inTex );
    return float4( pixelColor, 1.0f );
}

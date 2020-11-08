#pragma pack_matrix( row_major )

cbuffer ConstantBuffer : register( b0 )
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
    /*float gTime;
    float waterSpeed;
    float waterAmount;
    float waterHeight;*/
}

// Vertex Shader
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
    //input.inPos.z = sin( gTime * waterSpeed + ( input.inPos.x * input.inPos.y * waterAmount ) *
    //                cos( input.inPos.x * input.inPos.y * waterAmount ) ) * waterHeight;
    
    VS_OUTPUT output;
    output.outPos = mul( float4( input.inPos, 1.0f ), World );
    output.outPos = mul( output.outPos, View );
    output.outPos = mul( output.outPos, Projection );
    output.outTex = input.inTex;
    return output;
}

// Pixel Shader
struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTex : TEXCOORD;
};

Texture2D waterTexture : TEXTURE : register( t0 );
SamplerState samplerState : SAMPLER : register( s0 );

float4 PS( PS_INPUT input ) : SV_TARGET
{
    float3 pixelColor = waterTexture.Sample( samplerState, input.inTex );
    return float4( pixelColor, 1.0f );
}

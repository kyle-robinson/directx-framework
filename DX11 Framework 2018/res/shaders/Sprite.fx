#pragma pack_matrix( row_major )

cbuffer ObjectBuffer : register( b0 )
{
    float4x4 wvpMatrix;
}

struct VS_INPUT
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
};

struct VS_OUPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
};

VS_OUPUT VS( VS_INPUT input )
{
    VS_OUPUT output;
    output.outPosition = mul( float4( input.inPosition, 1.0f ), wvpMatrix );
    output.outTexCoord = input.inTexCoord;
    return output;
}

// pixel shader
cbuffer ColorBuffer : register( b1 )
{
    float alphaFactor;
    bool useTexture;
}

struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register( t0 );
SamplerState samplerState : SAMPLER : register( s0 );

float4 PS( PS_INPUT input ) : SV_TARGET
{
    float3 finalColor = objTexture.Sample( samplerState, input.inTexCoord );
    if ( useTexture )
    {
        clip( ( finalColor.r <= 0.5 && finalColor.r >= 0.2 ) ? -1 : 1 );
        clip( ( finalColor.g <= 0.5 && finalColor.g >= 0.2 ) ? -1 : 1 );
        clip( ( finalColor.b <= 0.5 && finalColor.b >= 0.2 ) ? -1 : 1 );
    }
    return float4( saturate( finalColor ), alphaFactor );
}
#pragma pack_matrix( row_major )

// vertex shader
cbuffer ObjectBuffer : register( b0 )
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer FogBuffer : register( b1 )
{
    float3 fogColor;
    float fogStart;
    float fogEnd;
    bool fogEnable;
}

struct VS_INPUT
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
    float  outFog : FOG;
};

VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output;
    
    float4x4 worldView = mul( worldMatrix, viewMatrix );
    float4x4 worldViewProj = mul( worldView, projectionMatrix );
    
    output.outPosition = mul( float4( input.inPosition, 1.0f ), worldViewProj );
    output.outWorldPos = (float3)mul( float4( input.inPosition, 1.0f ), worldMatrix );
    
    output.outTexCoord = input.inTexCoord;
    output.outNormal = mul( input.inNormal, (float3x3)worldMatrix );
    
    float3 viewPos = (float3)mul( float4( input.inPosition, 1.0f ), worldView );
    output.outFog = saturate( ( viewPos.z - fogEnd ) / ( fogEnd - fogStart ) ); // linear fog
    
    return output;
}

// pixel shader
cbuffer LightBuffer : register( b2 )
{
    float3 dynamicLightColor;
    float alphaFactor;
    bool useTexture;
};

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
    float  inFog : FOG;
};

Texture2D albedoTexture : DIFFUSE_TEXTURE : register( t0 );
SamplerState samplerState : SAMPLER : register( s0 );

float4 PS( PS_INPUT input ) : SV_TARGET
{   
    // sample from texture
    float3 albedoSample = albedoTexture.Sample( samplerState, input.inTexCoord );
    
    // directional lighting
    const float3 toLight = float3( 50.0f, 50.0f, -50.0f ) - input.inWorldPos;
    const float distanceToLight = length( toLight );
    const float3 directionToLight = toLight / distanceToLight;
    float NDotL = dot( directionToLight, input.inNormal );
    float3 directionalLight = dynamicLightColor * saturate( NDotL );
    
    // final color
    float3 finalColor = directionalLight * ( albedoSample = ( useTexture == true ) ? albedoSample : 1 );
    
    // fog factor
    const float fogValue = input.inFog * finalColor + ( 1.0 - input.inFog ) * fogColor;
    finalColor += ( fogEnable == true ) ? fogValue : 0;
    
    // output colour
    return float4( saturate( finalColor ), alphaFactor );
}